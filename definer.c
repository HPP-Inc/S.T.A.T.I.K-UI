/*
 * definer.c - STATIK UI Language Definer
 * For generic HTML UI, HPP 1.9 and lower
 *
 * This file provides basic definitions for STATIK UI elements.
 * Extend as needed for your application.
 */

#include <stdio.h>
#include <string.h>

#define MAX_ATTRS 10
#define MAX_ATTR_LEN 64
#define MAX_TAG_LEN 32

typedef struct {
    char name[MAX_ATTR_LEN];
    char value[MAX_ATTR_LEN];
} StatikAttr;

typedef struct {
    char tag[MAX_TAG_LEN];
    StatikAttr attrs[MAX_ATTRS];
    int attr_count;
    char inner_html[256];
} StatikElement;

// Create a new element
void statik_init(StatikElement *el, const char *tag) {
    strncpy(el->tag, tag, MAX_TAG_LEN - 1);
    el->tag[MAX_TAG_LEN - 1] = '\0';
    el->attr_count = 0;
    el->inner_html[0] = '\0';
}

// Add an attribute
void statik_add_attr(StatikElement *el, const char *name, const char *value) {
    if (el->attr_count < MAX_ATTRS) {
        strncpy(el->attrs[el->attr_count].name, name, MAX_ATTR_LEN - 1);
        el->attrs[el->attr_count].name[MAX_ATTR_LEN - 1] = '\0';
        strncpy(el->attrs[el->attr_count].value, value, MAX_ATTR_LEN - 1);
        el->attrs[el->attr_count].value[MAX_ATTR_LEN - 1] = '\0';
        el->attr_count++;
    }
}

// Set inner HTML
void statik_set_inner_html(StatikElement *el, const char *html) {
    strncpy(el->inner_html, html, sizeof(el->inner_html) - 1);
    el->inner_html[sizeof(el->inner_html) - 1] = '\0';
}

// Render element as HTML
void statik_render(const StatikElement *el, FILE *out) {
    fprintf(out, "<%s", el->tag);
    for (int i = 0; i < el->attr_count; ++i) {
        fprintf(out, " %s=\"%s\"", el->attrs[i].name, el->attrs[i].value);
    }
    fprintf(out, ">");
    if (el->inner_html[0]) {
        fprintf(out, "%s", el->inner_html);
    }
    fprintf(out, "</%s>\n", el->tag);
}

// Example usage
#ifdef STATIK_DEFINER_MAIN
int main() {
    StatikElement button;
    statik_init(&button, "button");
    statik_add_attr(&button, "class", "statik-btn");
    statik_add_attr(&button, "type", "submit");
    statik_set_inner_html(&button, "Click Me");
    statik_render(&button, stdout);
    return 0;
}
#endif