#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct Node
{
    int a;
    int b;
    struct Node *next;
} Node;

void nodeInit(Node *n, int a, int b, Node *next)
{
    n->a = a;
    n->b = b;
    n->next = next;
}

void destroyList(Node *head)
{
    Node *next = head->next;
    while (head != NULL)
    {
        next = head->next;
        free(head);
        head = next;
    }
}

int cmpByA(Node *n1, Node *n2)
{
    return n1->a < n2->a;
}

int cmpByB(Node *n1, Node *n2)
{
    return n1->b < n2->b;
}

Node *createList()
{
    Node *node1 = (Node *)malloc(sizeof(Node));
    Node *node2 = (Node *)malloc(sizeof(Node));
    Node *node3 = (Node *)malloc(sizeof(Node));
    Node *node4 = (Node *)malloc(sizeof(Node));
    Node *node5 = (Node *)malloc(sizeof(Node));
    Node *node6 = (Node *)malloc(sizeof(Node));
    nodeInit(node1, 1, 5, node2);
    nodeInit(node2, 7, 2, node3);
    nodeInit(node3, 3, 2, node4);
    nodeInit(node4, 4, 3, node5);
    nodeInit(node5, 9, 6, node6);
    nodeInit(node6, 2, 1, NULL);
    return node1;
}

void printNode(Node *n)
{
    printf("(%d, %d)->", n->a, n->b);
}

void printList(Node *head)
{
    while (head != NULL)
    {
        printNode(head);
        head = head->next;
    }
    printf("null\n");
}

Node *findMid(Node *list)
{
    // node1 node2 node3 --> mid = node2
    // node1 node2 node3 node4 --> mid = node2
    Node *fast = list;
    Node *slow = list;
    while (fast != NULL && fast->next != NULL && fast->next->next != NULL)
    {
        fast = fast->next->next;
        slow = slow->next;
    }
    return slow;
}

Node* listMerge(Node *left, Node *right, int (*cmp)(Node *, Node *))
{
    Node *dummyNode = (Node *)malloc(sizeof(Node));
    nodeInit(dummyNode, -1, -1, NULL);
    Node *head = dummyNode;
    while (left != NULL && right != NULL)
    {
        if (cmp(left, right))
        {
            head->next = left;
            left = left->next;
        }
        else
        {
            head->next = right;
            right = right->next;
        }
        head = head->next;
    }
    if (left != NULL)
        head->next = left;
    if (right != NULL)
        head->next = right;
    Node *ret = dummyNode->next;
    free(dummyNode);
    return ret;
}

Node *listMergeSort(Node *list, int (*cmp)(Node *, Node *))
{
    if (list == NULL || list->next == NULL)
        return list;
    // 利用快慢指针找到中间节点
    Node *mid = findMid(list);
    // 从中间节点节点断开链表
    Node *mid_next = mid->next;
    mid->next = NULL;
    // 归并排序左右两段链表
    Node *left = listMergeSort(list, cmp);
    Node *right = listMergeSort(mid_next, cmp);
    // 将排好序的左右链表归并
    return listMerge(left, right, cmp);
}

int main()
{
    Node *head = createList();
    printList(head);

    Node *sortedList = listMergeSort(head, cmpByA);
    printList(sortedList);

    sortedList = listMergeSort(head, cmpByB);
    printList(sortedList);
    destroyList(head);
    return 0;
}