#pragma once
#include <vector>

using namespace std;

template<typename K, typename D>
class Node {
public:
	K key;
	D* data;
	vector<D*> samekeydata;

	Node* left;
	Node* right;
	Node* parent;
	Node* newNode(K key, D* data, Node* parent);
	void insert(Node** head, K key, D* data);
	vector<D*> getNodesByKey(Node* root, K key);
};

using namespace std;

template<typename K, typename D>
Node<K, D>* Node<K, D>::newNode(K key, D* data, Node* parent)
{
	Node* tmp = new Node<K, D>();
	tmp->left = tmp->right = NULL;
	tmp->parent = parent;
	tmp->key = key;
	tmp->data = (D*)malloc(sizeof(D));
	tmp->data = data;
	return tmp;
}

template<typename K, typename D>
void Node<K, D>::insert(Node** head, K key, D* data)
{
	Node* tmp = NULL;
	Node* ins = NULL;
	if (*head == NULL)
	{
		*head = Node::newNode(key, data, NULL);
		return;
	}

	tmp = *head;
	while (tmp)
	{
		if (key > tmp->key)
		{
			if (tmp->right)
			{
				tmp = tmp->right;
				continue;
			}
			else
			{
				tmp->right = Node::newNode(key, data, tmp);
				return;
			}
		}
		else if (key < tmp->key)
		{
			if (tmp->left)
			{
				tmp = tmp->left;
				continue;
			}
			else
			{
				tmp->left = Node::newNode(key, data, tmp);
				return;
			}
		}
		else if (key == tmp->key)
		{
			tmp->samekeydata.push_back(data);
			return;
		}
	}

}

template<typename K, typename D>
vector<D*> Node<K, D>::getNodesByKey(Node* root, K key)
{
	vector<D*> lines;
	while (root)
	{
		if (root->key > key)
		{
			root = root->left;
			continue;
		}
		else if (root->key < key)
		{
			root = root->right;
			continue;
		}
		else
		{
			lines = root->samekeydata;
			lines.push_back(root->data);
			return lines;
		}
	}
	return lines;
}