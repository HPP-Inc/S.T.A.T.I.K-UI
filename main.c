/*
    Sataik UI Language Parser (Bare-bones HTML-like)
    This code defines a minimal parser for a generic UI language
    similar to HTML, supporting tags, attributes, and text nodes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TAG_NAME 32
#define MAX_ATTR_NAME 32
#define MAX_ATTR_VALUE 128
#define MAX_TEXT 256
#define MAX_CHILDREN 16
#define MAX_ATTRS 8

typedef struct SataikAttr {
        char name[MAX_ATTR_NAME];
        char value[MAX_ATTR_VALUE];
} SataikAttr;

typedef struct SataikNode {
        char tag[MAX_TAG_NAME];
        SataikAttr attrs[MAX_ATTRS];
        int attr_count;
        char text[MAX_TEXT];
        struct SataikNode* children[MAX_CHILDREN];
        int child_count;
} SataikNode;

// Utility: Skip whitespace
const char* skip_ws(const char* s) {
        while (isspace(*s)) s++;
        return s;
}

// Parse attribute name="value"
const char* parse_attr(const char* s, SataikAttr* attr) {
        s = skip_ws(s);
        int i = 0;
        while (*s && *s != '=' && !isspace(*s) && i < MAX_ATTR_NAME-1)
                attr->name[i++] = *s++;
        attr->name[i] = 0;
        s = skip_ws(s);
        if (*s == '=') s++;
        s = skip_ws(s);
        if (*s == '"' || *s == '\'') {
                char quote = *s++;
                i = 0;
                while (*s && *s != quote && i < MAX_ATTR_VALUE-1)
                        attr->value[i++] = *s++;
                attr->value[i] = 0;
                if (*s == quote) s++;
        } else {
                attr->value[0] = 0;
        }
        return s;
}

// Parse a tag and its children
const char* parse_node(const char* s, SataikNode* node) {
        memset(node, 0, sizeof(SataikNode));
        s = skip_ws(s);
        if (*s != '<') return NULL;
        s++;
        // Tag name
        int i = 0;
        while (*s && *s != '>' && !isspace(*s) && *s != '/' && i < MAX_TAG_NAME-1)
                node->tag[i++] = *s++;
        node->tag[i] = 0;
        // Attributes
        node->attr_count = 0;
        while (*s && *s != '>' && *s != '/') {
                s = skip_ws(s);
                if (*s != '>' && *s != '/' && node->attr_count < MAX_ATTRS) {
                        s = parse_attr(s, &node->attrs[node->attr_count++]);
                }
        }
        // Self-closing
        if (*s == '/') {
                s++;
                if (*s == '>') return s+1;
        }
        if (*s == '>') s++;
        // Children or text
        s = skip_ws(s);
        if (*s == '<' && s[1] == '/') {
                // Empty tag
                while (*s && *s != '>') s++;
                if (*s == '>') s++;
                return s;
        }
        // Text or children
        if (*s && *s != '<') {
                i = 0;
                while (*s && *s != '<' && i < MAX_TEXT-1)
                        node->text[i++] = *s++;
                node->text[i] = 0;
        }
        // Children
        node->child_count = 0;
        while (*s && *s == '<' && s[1] != '/') {
                if (node->child_count < MAX_CHILDREN) {
                        node->children[node->child_count] = malloc(sizeof(SataikNode));
                        s = parse_node(s, node->children[node->child_count]);
                        node->child_count++;
                } else {
                        break;
                }
                s = skip_ws(s);
        }
        // End tag
        if (*s == '<' && s[1] == '/') {
                while (*s && *s != '>') s++;
                if (*s == '>') s++;
        }
        return s;
}

// Print the parsed tree
void print_node(SataikNode* node, int depth) {
        for (int i = 0; i < depth; i++) printf("  ");
        printf("<%s", node->tag);
        for (int i = 0; i < node->attr_count; i++)
                printf(" %s=\"%s\"", node->attrs[i].name, node->attrs[i].value);
        printf(">");
        if (node->text[0]) printf("%s", node->text);
        printf("\n");
        for (int i = 0; i < node->child_count; i++)
                print_node(node->children[i], depth+1);
        for (int i = 0; i < depth; i++) printf("  ");
        printf("</%s>\n", node->tag);
}

// Example usage
int main() {
        const char* src =
                "<window title=\"Sataik UI\">"
                "  <label text=\"Hello, World!\"/>"
                "  <button text=\"Click Me\"/>"
                "</window>";
        SataikNode root;
        parse_node(src, &root);
        print_node(&root, 0);
        // Free children
        for (int i = 0; i < root.child_count; i++)
                free(root.children[i]);
        return 0;
}