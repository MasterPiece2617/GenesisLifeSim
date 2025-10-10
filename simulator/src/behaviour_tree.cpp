#include "behaviour_tree.hpp"

Node::Node(std::shared_ptr<Node> _root) : root(_root) {}

std::shared_ptr<Node> Node::get_root()
{
	return root;
}

std::vector<std::shared_ptr<Node>> Node::get_children()
{
	return children;
}

int Node::get_size()
{
	return size;
}

void Node::add_child(std::shared_ptr<Node> child)
{
	children.push_back(child);
	size++;
}

BTStatus Node::tick() 
{
	return BTStatus::FAILURE;
}

ControlNode::ControlNode(std::shared_ptr<Node> _root) : Node(_root) {}

int ControlNode::get_iterator()
{
	return i;
}

void ControlNode::reset()
{
	i = 0;
}

void ControlNode::add_child(std::shared_ptr<Node> child)
{
	Node::add_child(child);
	returned.resize(size);
}

ExecutionNode::ExecutionNode(std::shared_ptr<Node> _root, std::function<BTStatus()> _func) : Node(_root), func(_func) {}

BTStatus ExecutionNode::tick()
{
	return func();
}

SequenceNode::SequenceNode(std::shared_ptr<Node> _root) : ControlNode(_root) {}

BTStatus SequenceNode::tick()
{
	if (i > 0)
	{
		return returned[i - 1];
	}
	
	return BTStatus::FAILURE;
}

bool SequenceNode::iterate(BTStatus status)
{
	returned[i] = status;
	++i;

	if (status == BTStatus::FAILURE || status == BTStatus::RUNNING)
	{
		return true;
	}

	return false;
}

FallbackNode::FallbackNode(std::shared_ptr<Node> _root) : ControlNode(_root) {}

BTStatus FallbackNode::tick()
{
	if (i > 0)
	{
		return returned[i - 1];
	}

	return BTStatus::FAILURE;
}

bool FallbackNode::iterate(BTStatus status)
{
	returned[i] = status;
	++i;

	if (status == BTStatus::SUCCESS || status == BTStatus::RUNNING)
	{
		return true;
	}

	return false;
}

PararellNode::PararellNode(std::shared_ptr<Node> _root) : ControlNode(_root) {}

BTStatus PararellNode::tick()
{
	int success_count = 0;

	for (auto& state : returned)
	{
		if (state == BTStatus::SUCCESS)
		{
			++success_count;
		}
		else if (state == BTStatus::RUNNING)
		{
			return BTStatus::RUNNING;
		}
	}

	if (success_count >= success)
	{
		return BTStatus::SUCCESS;
	}

	return BTStatus::FAILURE;
}

bool PararellNode::iterate(BTStatus status)
{
	returned[i] = status;
	i++;

	return false;
}

DecoratorNode::DecoratorNode(std::shared_ptr<Node> _root) : ControlNode(_root) {}

void DecoratorNode::add_child(std::shared_ptr<Node> child)
{
	if (size == 1)
	{
		std::cerr << "Error: decorator has already a child";
		return;
	}

	children.push_back(child);
}

BTStatus DecoratorNode::tick()
{
	switch (policy)
	{
		case DecoratorPolicy::REPEAT:

			if (returned[0] == BTStatus::FAILURE)
			{
				return BTStatus::REPEAT;
			}
			
			return returned[0];

		default:

			return returned[0];
	}
}

bool DecoratorNode::iterate(BTStatus status)
{
	switch (policy)
	{
		case DecoratorPolicy::INVERT:

			if (status == BTStatus::RUNNING)
			{
				returned[0] = BTStatus::RUNNING;
			}

			if (status == BTStatus::SUCCESS)
			{
				returned[0] = BTStatus::FAILURE;
				break;
			}
			
			returned[0] = BTStatus::SUCCESS;

			break;

		default:

			returned[0] = status;
			break;
	}
	

	return false;
}

BehaviourTree::BehaviourTree(std::shared_ptr<Node> _root) : root(_root) 
{
	if (_root->get_root() != nullptr)
	{
		std::cerr << "Error: root node's root must be nullpt";
		exit(EXIT_FAILURE);
	}
}

void BehaviourTree::tick()
{
	std::stack<std::shared_ptr<Node>> stack;
	std::stack<std::shared_ptr<ControlNode>> control_stack;

	stack.push(root);

	while (!stack.empty())
	{
		std::shared_ptr<Node> node = stack.top();
		//std::cout << node << " evaluating\n";
		stack.pop();

		for (int i = node->get_size() - 1; i >= 0; --i)
		{
			//std::cout << node->get_children()[i] << " to stack\n";
			stack.push(node->get_children()[i]);
		}

		if (auto control_node = std::dynamic_pointer_cast<ControlNode>(node))
		{
			control_stack.push(control_node);
			continue;
		}

		if (control_stack.empty())
		{
			continue;
		}

		std::shared_ptr<ControlNode> control_node = control_stack.top();
		//std::cout << control_node << " control stack\n";

		if (control_node->get_iterator() >= control_node->get_size())
		{
			control_node->reset();
		}

		if (control_node->iterate(node->tick()))
		{
			for (int i = control_node->get_iterator() - 1; i < control_node->get_size() - 1; ++i)
			{
				if (!stack.empty())
				{
					stack.pop();
				}
			}

			BTStatus tick = control_node->tick();

			if (tick == BTStatus::REPEAT)
			{
				stack.push(control_node);
				continue;
			}

			if (!control_stack.empty())
			{
				control_stack.pop();
			}
			
			if (!control_stack.empty())
			{
				//std::cout << control_stack.top() << " " << control_stack.top()->get_iterator() << " of " << control_stack.top()->get_size() << " control stack\n";

				if (control_stack.top()->get_iterator() >= control_stack.top()->get_size())
				{
					control_stack.top()->reset();
				}

				control_stack.top()->iterate(tick);
			}
		}
	}
}

std::shared_ptr<Node> BehaviourTree::get_root()
{
	return root;
}