/**
 * @file Deque.c
 * 
 * @author Leonardo Vencovsky (https://github.com/LeoVen)
 *
 * @date 27/09/2018
 */

#include "Deque.h"

/// \brief A linked list implementation of a generic deque.
///
/// This is a linked list implementation of a Deque. A Deque is a double-ended
/// queue, so you can insert and remove elements from both ends of the queue.
/// It is implemented as a \c DoublyLinkedList but with restricted operations
/// which can only insert or remove elements at the edges of the linked list.
/// With this implementation every insertion and removal operations take
/// constant time O(1). This simplifies things a lot but every node needs to
/// maintain two pointers, one to its left and another to its right.
///
/// \b Advantages over \c DequeArray
/// - Indefinitely grows
/// - No need to reallocate buffers or shift elements
///
/// \b Drawbacks
/// - No random access
/// - More memory usage as in every node there are two pointers to the
/// neighbouring nodes
///
/// \b Functions
///
/// Located in the file Deque.c
struct Deque_s
{
    /// \brief Current amount of elements in the \c Deque.
    ///
    /// Deque current amount of nodes linked between the \c front and \c rear
    /// pointers.
    integer_t length;

    /// \brief Deque length limit.
    ///
    /// If it is set to 0 or a negative value then the deque has no limit to
    /// its length. Otherwise it won't be able to have more elements than the
    /// specified value. The deque is always initialized with no restrictions
    /// to its length, that is, \c limit equals 0. The user won't be able to
    /// limit the deque length if it already has more elements than the
    /// specified limit.
    integer_t limit;

    /// \brief Points to the first Node on the deque.
    ///
    /// Points to the first Node on the deque or \c NULL if the deque is empty.
    struct DequeNode_s *front;

    /// \brief Points to the last Node on the deque.
    ///
    /// Points to the first Node on the deque or \c NULL if the deque is empty.
    struct DequeNode_s *rear;

    /// \brief Comparator function.
    ///
    /// A function that compares one element with another that returns an int
    /// with the following rules:
    ///
    /// - <code>[ > 0 ]</code> if first element is greater than the second;
    /// - <code>[ < 0 ]</code> if second element is greater than the first;
    /// - <code>[ 0 ]</code> if elements are equal.
    deq_compare_f v_compare;

    /// \brief Copy function.
    ///
    /// A function that returns an exact copy of an element.
    deq_copy_f v_copy;

    /// \brief Display function.
    ///
    /// A function that displays an element in the console. Useful for
    /// debugging.
    deq_display_f v_display;

    /// \brief Deallocator function.
    ///
    /// A function that completely frees an element from memory.
    deq_free_f v_free;

    /// \brief A version id to keep track of modifications.
    ///
    /// This version id is used by the iterator to check if the structure was
    /// modified. The iterator can only function if its version_id is the same
    /// as the structure's version id, that is, there have been no structural
    /// modifications (except for those done by the iterator itself).
    integer_t version_id;
};

/// \brief A Deque_s node.
///
/// Implementation detail. This is a doubly-linked node with a pointer to the
/// previous node (or \c NULL if it is the front node) and another pointer to
/// the next node (or \c NULL if it is the rear node).
struct DequeNode_s
{
    /// \brief Data pointer.
    ///
    /// Points to node's data. The data needs to be dynamically allocated.
    void *data;

    /// \brief Next node on the deque.
    ///
    /// Next node on the deque or \c NULL if this is the rear node.
    struct DequeNode_s *next;

    /// \brief Previous node on the deque.
    ///
    /// Previous node on the deque or \c NULL if this is the front node.
    struct DequeNode_s *prev;
};

/// \brief A type for a deque node.
///
/// Defines a type to a <code> struct DequeNode_s </code>.
typedef struct DequeNode_s DequeNode_t;

/// \brief A pointer type for a deque node.
///
/// Defines a pointer type to a <code> struct DequeNode_s </code>.
typedef struct DequeNode_s *DequeNode;

///////////////////////////////////////////////////// NOT EXPOSED FUNCTIONS ///

static Status deq_make_node(DequeNode *node, void *data);

static Status deq_free_node(DequeNode *node, deq_free_f free_f);

static Status deq_free_node_shallow(DequeNode *node);

////////////////////////////////////////////// END OF NOT EXPOSED FUNCTIONS ///

/// \brief Initializes a Deque_s structure.
///
/// Initializes a new Deque_s structure with initial length and limit as 0.
/// Note that it does not sets any default functions. If you don't set them
/// later you won't be able to do certain operations that depend on a user-
/// defined function.
///
/// \param[in,out] deque The deque to be initialized.
///
/// \return DS_ERR_ALLOC if deque allocation failed.
/// \return DS_OK if all operations were successful.
Status deq_init(Deque *deque)
{
    (*deque) = malloc(sizeof(Deque_t));

    if (!(*deque))
        return DS_ERR_ALLOC;

    (*deque)->length = 0;
    (*deque)->limit = 0;
    (*deque)->version_id = 0;

    (*deque)->front = NULL;
    (*deque)->rear = NULL;

    (*deque)->v_compare = NULL;
    (*deque)->v_copy = NULL;
    (*deque)->v_display = NULL;
    (*deque)->v_free = NULL;

    return DS_OK;
}

/// \brief Creates a Deque_s.
///
/// This function completely creates a Deque_s, setting all of its default
/// functions.
///
/// \param[in,out] deque Deque_s to be allocated.
/// \param[in] compare_f A function that compares two elements.
/// \param[in] copy_f A function that makes an exact copy of an element.
/// \param[in] display_f A function that displays in the console an element.
/// \param[in] free_f A function that completely frees from memory an element.
///
/// \return DS_ERR_ALLOC if deque allocation failed.
/// \return DS_OK if all operations are successful.
Status deq_create(Deque *deque, deq_compare_f compare_f, deq_copy_f copy_f,
                  deq_display_f display_f, deq_free_f free_f)
{
    *deque = malloc(sizeof(Deque_t));

    if (!(*deque))
        return DS_ERR_ALLOC;

    (*deque)->length = 0;
    (*deque)->limit = 0;
    (*deque)->version_id = 0;

    (*deque)->front = NULL;
    (*deque)->rear = NULL;

    (*deque)->v_compare = compare_f;
    (*deque)->v_copy = copy_f;
    (*deque)->v_display = display_f;
    (*deque)->v_free = free_f;

    return DS_OK;
}

/// \brief Frees from memory a Deque_s and all its elements.
///
/// This function frees from memory all the deque's elements using its default
/// free function and then frees the deque's structure. The variable is then
/// set to \c NULL.
///
/// \param[in,out] deque The Deque_s to be freed from memory.
///
/// \return DS_ERR_INCOMPLETE_TYPE if a default free function is not set.
/// \return DS_ERR_NULL_POINTER if the deque reference is \c NULL.
/// \return DS_OK if all operations were successful.
Status deq_free(Deque *deque)
{
    if ((*deque) == NULL)
        return DS_ERR_NULL_POINTER;

    if ((*deque)->v_free == NULL)
        return DS_ERR_INCOMPLETE_TYPE;

    DequeNode prev = (*deque)->front;

    Status st;

    while ((*deque)->front != NULL)
    {
        (*deque)->front = (*deque)->front->prev;

        st = deq_free_node(&prev, (*deque)->v_free);

        if (st != DS_OK)
            return st;

        prev = (*deque)->front;
    }

    free((*deque));

    (*deque) = NULL;

    return DS_OK;
}

/// \brief Frees from memory a Deque_s.
///
/// This function frees from memory all the deque's nodes without freeing its
/// data and then frees the deque structure. The variable is then set to
/// \c NULL.
///
/// \param[in,out] deque Deque_s to be freed from memory.
///
/// \return DS_ERR_NULL_POINTER if the deque references to \c NULL.
/// \return DS_OK if all operations are successful.
Status deq_free_shallow(Deque *deque)
{
    if ((*deque) == NULL)
        return DS_ERR_NULL_POINTER;

    DequeNode prev = (*deque)->front;

    Status st;

    while ((*deque)->front != NULL)
    {
        (*deque)->front = (*deque)->front->prev;

        st = deq_free_node_shallow(&prev);

        if (st != DS_OK)
            return st;

        prev = (*deque)->front;
    }

    free((*deque));

    (*deque) = NULL;

    return DS_OK;
}

/// \brief Erases a Deque_s.
///
/// This function is equivalent to freeing a deque and the creating it again.
/// This will reset the deque to its initial state with no elements, but will
/// keep all of its default functions.
///
/// \param[in,out] deque Deque_s to be erased.
///
/// \return DS_ERR_ALLOC if deque allocation failed.
/// \return DS_ERR_INCOMPLETE_TYPE if a default free function is not set.
/// \return DS_ERR_NULL_POINTER if the deque reference is \c NULL.
/// \return DS_OK if all operations were successful.
Status deq_erase(Deque *deque)
{
    if (*deque == NULL)
        return DS_ERR_NULL_POINTER;

    Deque new_deque;

    Status st = deq_create(&new_deque, (*deque)->v_compare, (*deque)->v_copy,
            (*deque)->v_display, (*deque)->v_free);

    if (st !=  DS_OK)
        return st;

    st = deq_free(deque);

    // Probably didn't set the free function...
    if (st !=  DS_OK)
    {
        free(new_deque);

        return st;
    }

    *deque = new_deque;

    return DS_OK;
}

/// \brief Sets the default compare function.
///
/// Use this function to set a default compare function. It needs to comply
/// with the deq_compare_f specifications.
///
/// \param[in] deque Deque_s to set the default compare function.
/// \param[in] function A deq_compare_f kind of function.
///
/// \return DS_ERR_NULL_POINTER if the deque references to \c NULL.
/// \return DS_OK if all operations are successful.
Status deq_set_v_compare(Deque deque, deq_compare_f function)
{
    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    deque->v_compare = function;

    return DS_OK;
}

/// \brief Sets the default copy function.
///
/// Use this function to set a default compare function. It needs to comply
/// with the deq_copy_f specifications.
///
/// \param[in] deque Deque_s to set the default copy function.
/// \param[in] function A deq_copy_f kind of function.
///
/// \return DS_ERR_NULL_POINTER if the deque references to \c NULL.
/// \return DS_OK if all operations are successful.
Status deq_set_v_copy(Deque deque, deq_copy_f function)
{
    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    deque->v_copy = function;

    return DS_OK;
}

/// \brief Sets the default display function.
///
/// Use this function to set a default display function. It needs to comply
/// with the deq_display_f specifications. Useful for debugging.
///
/// \param[in] deque Deque_s to set the default display function.
/// \param[in] function A deq_display_f kind of function.
///
/// \return DS_ERR_NULL_POINTER if the deque references to \c NULL.
/// \return DS_OK if all operations are successful.
Status deq_set_v_display(Deque deque, deq_display_f function)
{
    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    deque->v_display = function;

    return DS_OK;
}

/// \brief Sets the default free function.
///
/// Use this function to set a default free function. It needs to comply
/// with the deq_free_f specifications.
///
/// \param[in] deque Deque_s to set the default free function.
/// \param[in] function A deq_free_f kind of function.
///
/// \return DS_ERR_NULL_POINTER if the deque references to \c NULL.
/// \return DS_OK if all operations are successful.
Status deq_set_v_free(Deque deque, deq_free_f function)
{
    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    deque->v_free = function;

    return DS_OK;
}

/// \brief Sets a limit to the specified Deque_s's length.
///
/// Limit's the Deque_s's length. You can only set a limit greater or equal to
/// the deque's current length and greater than 0. To remove this limitation
/// simply set the limit to 0 or less.
///
/// \param[in] deque Deque_s reference.
/// \param[in] limit Maximum deque length.
///
/// \return DS_ERR_INVALID_OPERATION if the limitation is less than the deque's
/// current length.
/// \return DS_ERR_NULL_POINTER if the deque references to \c NULL.
/// \return DS_OK if all operations are successful.
Status deq_set_limit(Deque deque, integer_t limit)
{
    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    // The new limit can't be lower than the deque's current length.
    if (deque->length > limit && limit > 0)
        return DS_ERR_INVALID_OPERATION;

    deque->limit = limit;

    return DS_OK;
}

integer_t deq_length(Deque deque)
{
    if (deque == NULL)
        return -1;

    return deque->length;
}

integer_t deq_limit(Deque deque)
{
    if (deque == NULL)
        return -1;

    return deque->limit;
}

/// Inserts an element at the front of the specified deque.
///
/// \param[in] deque The deque where the element is to be inserted.
/// \param[in] element The element to be inserted in the deque.
///
/// \return DS_ERR_ALLOC if node allocation failed.
/// \return DS_ERR_FULL if \c limit is set (greater than 0) and the deque
/// length reached the specified limit.
/// \return DS_ERR_NULL_POINTER if deque reference is \c NULL.
/// \return DS_OK if all operations were successful.
Status deq_enqueue_front(Deque deque, void *element)
{
    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    if (deq_full(deque))
        return DS_ERR_FULL;

    DequeNode node;

    Status st = deq_make_node(&node, element);

    if (st != DS_OK)
        return st;

    if (deq_empty(deque))
    {
        deque->front = node;
        deque->rear = node;
    }
    else
    {
        node->prev = deque->front;

        deque->front->next = node;
        deque->front = node;
    }

    deque->length++;
    deque->version_id++;

    return DS_OK;
}

/// Inserts an element at the rear of the specified deque.
///
/// \param[in] deque The deque where the element is to be inserted.
/// \param[in] element The element to be inserted in the deque.
///
/// \return DS_ERR_ALLOC if node allocation failed.
/// \return DS_ERR_FULL if \c limit is set (greater than 0) and the deque
/// length reached the specified limit.
/// \return DS_ERR_NULL_POINTER if deque reference is \c NULL.
/// \return DS_OK if all operations were successful.
Status deq_enqueue_rear(Deque deque, void *element)
{
    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    if (deq_full(deque))
        return DS_ERR_FULL;

    DequeNode node;

    Status st = deq_make_node(&node, element);

    if (st != DS_OK)
        return st;

    if (deq_empty(deque))
    {
        deque->front = node;
        deque->rear = node;
    }
    else
    {
        node->next = deque->rear;

        deque->rear->prev = node;
        deque->rear = node;
    }

    deque->length++;
    deque->version_id++;

    return DS_OK;
}

/// Removes an element from the front of the specified deque.
///
/// \param[in] deque The deque where the element is to be removed from.
/// \param[out] result The resulting element removed from the deque.
///
/// \return DS_ERR_INVALID_OPERATION if the deque is empty.
/// \return DS_ERR_NULL_POINTER if deque reference is \c NULL.
/// \return DS_OK if all operations were successful.
Status deq_dequeue_front(Deque deque, void **result)
{
    *result = NULL;

    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    if (deq_empty(deque))
        return DS_ERR_INVALID_OPERATION;

    DequeNode node = deque->front;

    *result = node->data;

    deque->front = deque->front->prev;

    if (deque->front == NULL)
        deque->rear = NULL;
    else
        deque->front->next = NULL;

    Status st = deq_free_node_shallow(&node);

    if (st != DS_OK)
        return st;

    deque->length--;
    deque->version_id++;

    return DS_OK;
}

/// Removes an element from the rear of the specified deque.
///
/// \param[in] deque The deque where the element is to be removed from.
/// \param[out] result The resulting element removed from the deque.
///
/// \return DS_ERR_INVALID_OPERATION if the deque is empty.
/// \return DS_ERR_NULL_POINTER if deque reference is \c NULL.
/// \return DS_OK if all operations were successful.
Status deq_dequeue_rear(Deque deque, void **result)
{
    *result = NULL;

    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    if (deq_empty(deque))
        return DS_ERR_INVALID_OPERATION;

    DequeNode node = deque->rear;

    *result = node->data;

    deque->rear = deque->rear->next;

    if (deque->rear == NULL)
        deque->front = NULL;
    else
        deque->rear->prev = NULL;

    Status st = deq_free_node_shallow(&node);

    if (st != DS_OK)
        return st;

    deque->length--;
    deque->version_id++;

    return DS_OK;
}

bool deq_full(Deque deque)
{
    return deque->limit > 0 && deque->length >= deque->limit;
}

bool deq_empty(Deque deque)
{
    return deque->length == 0;
}

void *deq_peek_front(Deque deque)
{
    if (deque == NULL)
        return NULL;

    if (deq_empty(deque))
        return NULL;

    return deque->front->data;
}

void *deq_peek_rear(Deque deque)
{
    if (deque == NULL)
        return NULL;

    if (deq_empty(deque))
        return NULL;

    return deque->rear->data;
}

bool deq_contains(Deque deque, void *key)
{
    // TODO
    return false;
}

Status deq_copy(Deque deque, Deque *result)
{
    *result = NULL;

    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    if (deque->v_copy == NULL || deque->v_free == NULL)
        return DS_ERR_INCOMPLETE_TYPE;

    Status st = deq_create(result, deque->v_compare, deque->v_copy,
                           deque->v_display, deque->v_free);

    if (st != DS_OK)
        return st;

    (*result)->limit = deque->limit;

    if (deq_empty(deque))
        return DS_OK;

    DequeNode scan = deque->front;

    void *elem;

    while (scan != NULL)
    {
        elem = deque->v_copy(scan->data);

        st = deq_enqueue_rear(*result, elem);

        if (st != DS_OK)
        {
            deque->v_free(elem);

            return st;
        }

        scan = scan->prev;
    }

    return DS_OK;
}

Status deq_append(Deque deque1, Deque deque2)
{
    // TODO
    return DS_ERR_INVALID_OPERATION;
}

Status deq_prepend(Deque deque1, Deque deque2)
{
    // TODO
    return DS_ERR_INVALID_OPERATION;
}

Status deq_to_array(Deque deque,  void ***result, integer_t *length)
{
    // TODO
    return DS_ERR_INVALID_OPERATION;
}

/// \brief Displays a Deque_s in the console.
///
/// Displays a Deque_s in the console starting from \c front to \c rear.
///
/// \param[in] deque The Deque_s to be displayed in the console.
///
/// \return DS_ERR_INCOMPLETE_TYPE if a default display function is not set.
/// \return DS_ERR_NULL_POINTER if the deque references to \c NULL.
/// \return DS_OK if all operations were successful.
Status deq_display(Deque deque)
{
    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    if (deque->v_display == NULL)
        return DS_ERR_INCOMPLETE_TYPE;

    if (deq_empty(deque))
    {
        printf("\nDeque\n[ empty ]\n");

        return DS_OK;
    }

    DequeNode scan = deque->front;

    printf("\nDeque\nfront <-> ");

    while (scan != NULL)
    {
        deque->v_display(scan->data);

        printf(" <-> ");

        scan = scan->prev;
    }

    printf(" rear\n");

    return DS_OK;
}

/// \brief Displays a Deque_s in the console like an array.
///
/// Displays a Deque_s in the console starting from \c front to \c rear like an
/// array with its elements separated by commas, delimited with brackets.
///
/// \param[in] deque The Deque_s to be displayed in the console.
///
/// \return DS_ERR_INCOMPLETE_TYPE if a default display function is not set.
/// \return DS_ERR_NULL_POINTER if the deque references to \c NULL.
/// \return DS_OK if all operations were successful.
Status deq_display_array(Deque deque)
{
    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    if (deque->v_display == NULL)
        return DS_ERR_INCOMPLETE_TYPE;

    if (deq_empty(deque))
    {
        printf("\n[ empty ]\n");

        return DS_OK;
    }

    DequeNode scan = deque->front;

    printf("\n[ ");

    while (scan->prev != NULL)
    {
        deque->v_display(scan->data);

        printf(", ");

        scan = scan->prev;
    }

    deque->v_display(scan->data);

    printf(" ]\n");

    return DS_OK;
}

/// \brief Displays a Deque_s in the console.
///
/// Displays a Deque_s in the console starting from \c front to \c rear with
/// its elements separated by spaces.
///
/// \param[in] deque The Deque_s to be displayed in the console.
///
/// \return DS_ERR_INCOMPLETE_TYPE if a default display function is not set.
/// \return DS_ERR_NULL_POINTER if the deque references to\c NULL.
/// \return DS_OK if all operations were successful.
Status deq_display_raw(Deque deque)
{
    if (deque == NULL)
        return DS_ERR_NULL_POINTER;

    if (deque->v_display == NULL)
        return DS_ERR_INCOMPLETE_TYPE;

    printf("\n");

    if (deq_empty(deque))
        return DS_OK;

    DequeNode scan = deque->front;

    while (scan != NULL)
    {
        deque->v_display(scan->data);

        printf(" ");

        scan = scan->prev;
    }

    printf("\n");

    return DS_OK;
}

///////////////////////////////////////////////////// NOT EXPOSED FUNCTIONS ///

static Status deq_make_node(DequeNode *node, void *data)
{
    (*node) = malloc(sizeof(DequeNode_t));

    if (!(*node))
        return DS_ERR_ALLOC;

    (*node)->data = data;

    (*node)->prev = NULL;
    (*node)->next = NULL;

    return DS_OK;
}

static Status deq_free_node(DequeNode *node, deq_free_f free_f)
{
    if ((*node) == NULL)
        return DS_ERR_NULL_POINTER;

    free_f((*node)->data);

    free(*node);

    (*node) = NULL;

    return DS_OK;
}

static Status deq_free_node_shallow(DequeNode *node)
{
    if (*node == NULL)
        return DS_ERR_NULL_POINTER;

    free(*node);

    *node = NULL;

    return DS_OK;
}

////////////////////////////////////////////// END OF NOT EXPOSED FUNCTIONS ///

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// Iterator ///
///////////////////////////////////////////////////////////////////////////////

// TODO