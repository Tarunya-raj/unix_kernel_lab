#ifndef LIST_H
#define LIST_H
struct ListNode
{
    struct ListNode* next;
    struct ListNode* prev;
};
inline void list_add(struct ListNode* newNode, struct ListNode* leftNode, struct ListNode* rightNode )
{
    newNode->next= rightNode;
    newNode->prev= leftNode;
    leftNode->next= newNode;
    rightNode->prev=newNode;
}
inline void list_init(struct ListNode* head)
{
    head->next= head;
    head->prev= head;
}
inline bool list_empty(const struct ListNode* head)
{
    return head->next == head;
}
// Insert immediately after head
inline void list_add_head(struct ListNode* node, struct ListNode* head)
{
   
    list_add(node, head, head->next);
}
// Insert immediately before head
inline void list_add_tail(ListNode* node, ListNode* head)
{
    list_add(node, head->prev, head);

}



inline void list_del(ListNode* node)
{
    struct ListNode* prev = node->prev;
    struct ListNode* next= node->next;
    prev->next= node->next;
    next->prev= node->prev;
    list_init(node);

}
#endif