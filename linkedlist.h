/*
    Node
*/
typedef struct node node;

typedef struct node
{
    ListElementType element;
    node *next;
} node;

/*
    LinkedList
*/
typedef struct
{
    node *root;
    node *cur;
} singleLinkedList;

/*------------Singly linked list functions--------------*/

//initializing function
singleLinkedList* createSingleList();
node *makeNewNode(ListElementType e);

//Insert functions
node* insertBegin(singleLinkedList *list, ListElementType e);
node* insertEnd(singleLinkedList *list, ListElementType e);
node* insertAtPosition(singleLinkedList *list, ListElementType e, int n);

//delete function
node* deleteBegin(singleLinkedList *list);
node* deleteEnd(singleLinkedList *list);
node* deleteSingleList(singleLinkedList *list);

// get linked list's length function
int listLength(singleLinkedList list);



/*-------------------------Implementing functions-------------------*/

/*
    createSingleList: Create new single linked list
*/
singleLinkedList* createSingleList()
{
    singleLinkedList *list = malloc(sizeof(singleLinkedList));
    list->root = NULL;
    list->cur = list->root;

    return list;
}

/*
    makeNewNode: Create new node
*/
node * makeNewNode(ListElementType element)
{
    node * new = (node *)malloc(sizeof(node));              
    new->element = element;
    new->next = NULL;

    return new;
}

/*
    insertEnd: Add element to the end of a linked list
*/
node* insertEnd(singleLinkedList *list, ListElementType element)
{
    node *new = makeNewNode(element);                       // create new node with value: element

    if (list->root == NULL) {                               // case: empty list
        list->root = list->cur = new;

    } else {                                                // case: not empty list
        list->cur = list->root;

        while (list->cur->next != NULL) {
            list->cur = list->cur->next;                    // traverse till the last node
        }

        list->cur->next = new;                              // add new node
    }

    return new;
}

/*
    Add element to head of a list
*/

node* insertBegin(singleLinkedList *list, ListElementType element)
{
    node *new= makeNewNode(element);                        // create new node with value: element

    if (list->root == NULL) {                               // case: empty list
        list->root = new;
        list->cur = (*list).root;

    } else {                                                // case: not empty list
        new->next = list->root;
        list->root = new;
        list->cur = new;
    }

    return list->cur;
}

/*
    insertAtPosition: Add element to position pos
*/

node* insertAtPosition(singleLinkedList *list, ListElementType element, int pos)
{
    node *new = makeNewNode(element);

    if (list->root == NULL) {                               // case: empty list
        list->root = new;
        list->cur = list->root;

    } else {
        if (pos <= 1) {                                     // case: insert head
            return insertBegin(list, new->element);
        }

        if (pos > listLength(*list)) {                      // case: position is bigger than list length
            return insertEnd(list, new->element);

        } else {
            list->cur = list->root;             

            int i = 0;

            while ((list->cur->next != NULL) && (i < pos)) { // case: position is in range
                i++;

                if (i == pos) {                             // traverse till the position
                    node *nextNode = list->cur->next;
                    list->cur->next = new;                  // add new node
                    new->next = nextNode;
                }

                list->cur = list->cur->next;
            }
        }
    }

    return list->cur;
}
/*
    deleteBegin: Delete first root in list and return new root
*/

node* deleteBegin(singleLinkedList *list)
{
    if (list->root == NULL) {                       // case : empty list
        return NULL;
    }

    if (list->root != NULL) {                       // case: not empty linked list

        list->cur = list->root;

        list->root = list->root->next;

        free(list->cur);                            // free memory

        list->cur = list->root;                     // make cur pointer point to new root and return
    }

    return list->cur;
}

/*
    deleteEnd: Delete last root in list
*/

node* deleteEnd(singleLinkedList *list)
{
    if (list->root == NULL) {                       // case : empty linked list
        return NULL;
    }

    if (list->root->next == NULL) {                 // case: linked list has only one element
        list->cur = list->root;

        free(list->cur);
        return NULL;
    }

    if (list->root != NULL) {                       // case: linked list has more than one element

        list->cur = list->root;
        node* prev = NULL;                          // prev pointer save previous state to return

        while (list->cur->next != NULL) {
            prev = list->cur;
            list->cur = list->cur->next;
        }
        prev->next = NULL;

        free(list->cur);                            // free memory

        return prev;
    }

    return NULL;
}

/*
    deleteSingleList: Delete all element in list
*/

node *deleteSingleList(singleLinkedList *list)
{
    while (list->root != NULL)
        deleteBegin(list);
    return NULL;
}

/*
    Get length of the list
*/

int listLength(singleLinkedList list) {
    int count = 0;                          // Initialize count
    list.cur = list.root;  

    while (list.cur != NULL) {              // travelse till end of list

        count++;
        list.cur = list.cur->next;
    }

    return count;
}
