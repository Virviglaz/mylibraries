/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2019 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Linked list generic api
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "list.h"

static void *(*_malloc)(size_t size) = NULL;
static void (*_free)(void *pointer) = NULL;

static struct list_t *find_by_num(struct list_t *list, size_t num)
{
	size_t i = 0;

	while (list->next) {
		if (i == num)
			return list;

		list = list->next;
		i++;
	}

	return NULL;
}

static size_t find_num(struct list_t *list, struct list_t *what)
{
	size_t i = 0;

	while (list->next) {
		if (list == what)
			return i;
		list = list->next;
		i++;
	}

	return 0;
}

static struct list_t *find_by_entry(struct list_t *list, void *entry)
{
	while (list->next) {
		if (list->entry == entry)
			return list;

		list = list->next;
	}

	return NULL;
}

static struct list_t *find_last(struct list_t *list)
{
	while (list->next)
		list = list->next;

	return list;
}

static struct list_t *find_prev(struct list_t *list, struct list_t *what)
{
	size_t num = find_num(list, what);
	if (!num)
		return NULL;

	return find_by_num(list, num - 1);
}

static void remove_first(struct list_t *list)
{
	struct list_t *next = list->next;

	if (!list->next) { /* Only one entry in list */
		list->next = NULL;
		return;
	}

	list->entry = next->entry;
	list->next = next->next;
	
	_free(next);
}

struct list_t *create_list(void *(*__malloc)(size_t size),
	void (*__free)(void *pointer))
{
	struct list_t *list;

	/* Register an external memory manager i.e. malloc */
	if (__malloc && __free) {
		_malloc = __malloc;
		_free = __free;
	}

	/* If no memory manager registered, can't procced */
	if (!_malloc || !_free)
		return NULL;

	/* Create a linked list and terminate the last */
	list = _malloc(sizeof(*list));
	if (!list)
		return NULL;

	list->next = NULL;;

	return list;
}

struct list_t *add_to_list(struct list_t *list, void *entry)
{
	struct list_t *last = find_last(list);
	if (!last)
		return NULL;

	last->entry = entry;
	last->next = _malloc(sizeof(*list));
	last = last->next;
	last->next = NULL;

	return last;
}

size_t get_list_size(struct list_t *list)
{
	size_t i = 0;

	while (list->next) {
		list = list->next;
		i++;
	}

	return i;
}

/* NOTE: We do not release the entry used memory */
void remove_entry_from_list(struct list_t *list, void *entry)
{
	struct list_t *prev;
	struct list_t *to_remove = find_by_entry(list, entry);
	if (!to_remove)
		return;

	prev = find_prev(list, to_remove);
	if (!prev) { /* First element */
		remove_first(list);
		return;
	}

	prev->next = to_remove->next;
	_free(to_remove);
}

void remove_entry_by_num(struct list_t *list, size_t num)
{
	remove_entry_from_list(list, get_entry_by_num(list, num));
}

void *get_entry_by_num(struct list_t *list, size_t num)
{
	list = find_by_num(list, num);

	return list->entry;
}

void *foreach_list(struct list_t *list)
{
	static struct list_t *last = NULL;

	if (!list && !last)
		return NULL;

	if (list) {
		last = list;
		return list->entry;
	}

	last = last->next;
	if (!last)
		return NULL;

	return last->entry;
}

void execute_foreach(struct list_t *list,
		void(*handler)(void *entry))
{
	while (list->next) {
		handler(list->entry);
		list = list->next;
	}
}

void release_list(struct list_t *list)
{
	while (list->next)
		remove_first(list);
}
