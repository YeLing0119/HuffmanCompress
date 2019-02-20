#include <iostream>
#include <vector>
#include <queue>

template<class HW>
struct HuffTreeNode{
    HuffTreeNode(const HW &weight):huffWeight(weight),pLeft(nullptr),pRight(nullptr),pParent(nullptr){
    }
        
    HuffTreeNode<HW> *pLeft;
    HuffTreeNode<HW> *pRight;
    HuffTreeNode<HW> *pParent;
    HW huffWeight;
};

template<class HW>
struct Compare{
    bool operator()(HuffTreeNode<HW>* pLeft, HuffTreeNode<HW>* pRight){
        if(pLeft->huffWeight > pRight->huffWeight){
            return true;
        }
        return false;
    }
};


template<class HW>
class HuffmanTree{
    typedef HuffTreeNode<HW> Node;       
    typedef HuffTreeNode<HW>* pNode;
    private:
        pNode pRoot;
    public:
        HuffmanTree():pRoot(nullptr){

        }

        void CreateHuffmanTree(const std::vector<HW> &HWeight, const HW& invalid){
            if(HWeight.empty()){
                std::cout << "Empty HWeight" << std::endl;
                return;
            }
            
            // 创建二叉树森林, 并放在小堆中
            std::priority_queue<pNode, std::vector<pNode>, Compare<HW>> prio_queue;

            for(size_t i = 0; i < HWeight.size(); ++i){
				if(HWeight[i] != invalid){
					prio_queue.push(new Node(HWeight[i]));
				}
            }

            while(prio_queue.size() > 1){
                pNode pLeft = prio_queue.top();
                prio_queue.pop();

                pNode pRight = prio_queue.top();
                prio_queue.pop();

                pNode pParent = new Node(pLeft->huffWeight + pRight->huffWeight);
                pParent->pLeft = pLeft;
                pLeft->pParent = pParent;

                pParent->pRight = pRight;
                pRight->pParent = pParent;

                prio_queue.push(pParent);
            }
            pRoot = prio_queue.top();
        }

        pNode GetRoot(){
            return pRoot;
        }

		unsigned long long GetRootWeight(){
			return pRoot->huffWeight.count;
		}

        ~HuffmanTree(){
            Destory(pRoot);
        }
    private:
        void Destory(pNode &pRoot){
            if(pRoot){
                Destory(pRoot->pLeft);
                Destory(pRoot->pRight);
                delete pRoot;
                pRoot = nullptr;
            }
        }
};


