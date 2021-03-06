/*
 * Copyright (c) 2010 Julien Palard.  All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "logtop.h"

static int	compare_string(const void *element1, const void *element2)
{
    return (strcmp(((log_entry_t*)element1)->string, ((log_entry_t*)element2)->string));
}

static int	compare_count(const void *element1, const void *element2)
{
    if (((log_entry_t*)element1)->count == ((log_entry_t*)element2)->count)
	return (long)element1 - (long)element2;
    return (((log_entry_t*)element2)->count - ((log_entry_t*)element1)->count);
}

static void	freeitem(void *element)
{
    free(((log_entry_t*)element)->string);
    free(element);
}

void	init_data_structures(int size)
{
    gl_env.history = (log_entry_t**)calloc(sizeof(log_entry_t*), size);
    gl_env.history_index = 0;
    gl_env.history_size = size;
    gl_env.strings = avl_alloc_tree(compare_string, freeitem);
    gl_env.top = avl_alloc_tree(compare_count, NULL);
    if (gl_env.strings == NULL || gl_env.top == NULL)
    {
	fputs("Not enough memory to create storage", stderr);
	exit(EXIT_FAILURE);
    }
}

void die()
{
    fputs("Ran out of memory, commit suicide for important tasks to live !", stderr);
    exit(EXIT_FAILURE);
}

log_entry_t	*create_log_entry(char *string)
{
    log_entry_t	*entry;

    entry = (log_entry_t*)malloc(sizeof(log_entry_t));
    if (entry == NULL)
	die();
    entry->count = 0;
    entry->string = strdup(string);
    if (entry->string == NULL)
	die();
    entry->string_node = avl_insert(gl_env.strings, entry);
    entry->top_node = avl_insert(gl_env.top, entry);
    return entry;
}

log_entry_t	*get_log_entry(char *string)
{
    avl_node_t	*node;
    log_entry_t	search;

    search.string = string;
    node = avl_search(gl_env.strings, &search);
    if (node != NULL)
	return (log_entry_t*)node->item;
    else
	return create_log_entry(string);
}

void	update_log_entry(log_entry_t *log_entry)
{
    avl_unlink_node(gl_env.top, log_entry->top_node);
    avl_insert_node(gl_env.top, log_entry->top_node);
}
