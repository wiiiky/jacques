/*
 * Copyright (C) 2015 Wiky L
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#ifndef __JAC_LIST_H__
#define __JAC_LIST_H__


/*
 * 双向列表
 */

typedef struct _DList DList;

struct _DList {
    void *data;

    DList *prev;
    DList *next;
};
#define dlist_new() NULL
#define dlist_data(l)   ((l)->data)
#define dlist_prev(l)   ((l)->prev)
#define dlist_next(l)   ((l)->next)

/*
 * 在列表末尾添加元素
 */
DList *dlist_append(DList *l, void *ptr);

/*
 * 在列表开头添加元素
 */
DList *dlist_prepend(DList *l, void *ptr);

/*
 * 从列表中删除指定元素
 */
DList *dlist_remove(DList *l, void *ptr);


#endif
