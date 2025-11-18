#pragma once

#include <iostream>
#include <functional>
#include <memory>
#include <stack>
#include <vector>

enum class BTStatus
{
	SUCCESS, FAILURE, RUNNING, REPEAT
};

enum class DecoratorPolicy
{
	INVERT, REPEAT
};

// class Scene; // Forward declaration

class Node
{
protected:
	std::shared_ptr<Node> root;
	std::vector<std::shared_ptr<Node>> children;
	int size = 0;
public:
	Node(std::shared_ptr<Node> _root);
	std::shared_ptr<Node> get_root();
	std::vector<std::shared_ptr<Node>> get_children();
	int get_size();
	virtual void add_child(std::shared_ptr<Node> child);
	virtual BTStatus tick();
};


class ExecutionNode : public Node
{
protected:
	std::function<BTStatus()> func;
public:
	ExecutionNode(std::shared_ptr<Node> _root, std::function<BTStatus()> func);
	virtual BTStatus tick() override;
};

class ControlNode : public Node
{
protected:
	int i = 0;
	std::vector<BTStatus> returned = std::vector<BTStatus>();
public:
	ControlNode(std::shared_ptr<Node> _root);
	int get_iterator();
	void reset();
	virtual void add_child(std::shared_ptr<Node> child) override;
	virtual BTStatus tick() = 0;
	virtual bool iterate(BTStatus status) = 0;
};

class SequenceNode : public ControlNode
{
public:
	SequenceNode(std::shared_ptr<Node> _root);
	virtual BTStatus tick() override;
	virtual bool iterate(BTStatus status) override;
};

class FallbackNode : public ControlNode
{
public:
	FallbackNode(std::shared_ptr<Node> _root);
	virtual BTStatus tick() override;
	virtual bool iterate(BTStatus status) override;
};

class PararellNode : public ControlNode
{
protected:
	int success = 1;
public:
	PararellNode(std::shared_ptr<Node> _root);
	virtual BTStatus tick() override;
	virtual bool iterate(BTStatus status) override;
};

class DecoratorNode : public ControlNode
{
protected:
	DecoratorPolicy policy;
public:
	DecoratorNode(std::shared_ptr<Node> _root);
	virtual void add_child(std::shared_ptr<Node> child) override;
	virtual BTStatus tick() override;
	virtual bool iterate(BTStatus status) override;
};

class BehaviourTree
{
protected:
	std::shared_ptr<Node> root;
	BTStatus state = BTStatus::SUCCESS;

public:
	BehaviourTree(std::shared_ptr<Node> _root);
	std::shared_ptr<Node> get_root();
	void tick();
};