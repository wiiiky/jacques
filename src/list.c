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
#include "list.h"
#include <stdlib.h>

static inline DList *dlist_alloc(void *ptr) {
    DList *l=malloc(sizeof(DList));
    l->data=ptr;
    l->prev=NULL;
    l->next=NULL;
    return l;
}

/*
 * 在列表末尾添加元素
 */
DList *dlist_append(DList *l, void *ptr) {
    DList *e=dlist_alloc(ptr);
    if(l==NULL) {
        return e;
    }
    DList *last=l;
    while(dlist_next(last)) {
        last=dlist_next(last);
    }
    last->next=e;
    e->prev=last;
    return l;
}

/*
 * 在列表开头添加元素
 */
DList *dlist_prepend(DList *l, void *ptr) {
    DList *e=dlist_alloc(ptr);
    if(l==NULL) {
        return e;
    }
    DList *first=l;
    while(dlist_prev(first)) {
        first=dlist_prev(first);
    }
    first->prev=e;
    e->next=first;
    return e;
}

/*
 * 从列表中删除指定元素
 */
DList *dlist_remove(DList *l, void *ptr) {
    DList *p=l;
    while(p) {
        if(dlist_data(p)==ptr) {
            break;
        }
        p=dlist_next(p);
    }
    if(p==NULL) {
        return l;
    }
    DList *prev=dlist_prev(p);
    DList *next=dlist_next(p);
    if(prev) {
        prev->next=next;
    }
    if(next) {
        next->prev=prev;
    }
    if(prev) {
        return l;
    }
    return next;
}
