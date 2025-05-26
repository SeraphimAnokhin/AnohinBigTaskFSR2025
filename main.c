#include "stdio.h"
#include "stdlib.h"
#include "lodepng.h"
#include "math.h"
#include "string.h"


typedef struct adj_list_elem {
    int v;
    struct adj_list_elem *next;
} adj_list_elem;


typedef struct dsu_node {
    int v;
    int rang;
    struct dsu_node *parent;
} dsu_node;


dsu_node * dsu_get(dsu_node *n) {
    if (n->parent == n) {
        return n;
    }
    return n->parent = dsu_get(n->parent);
}


dsu_node * dsu_union(dsu_node *a, dsu_node *b) {
    a = dsu_get(a);
    b = dsu_get(b);
    if (a == b) {
        return a;
    }
    if (a->rang == b->rang) {
        a->rang++;
    }
    if (a->rang > b->rang) {
        return b->parent = a;
    }
    return a->parent = b;
}


int find_components(int n, adj_list_elem **adj_lists, int *comp, int *comp_size) {
    int i, j;
    dsu_node *vert[n];
    for (i = 0; i < n; i++) {
        vert[i] = malloc(sizeof(dsu_node));
        vert[i]->parent = vert[i];
        vert[i]->v = i;
        vert[i]->rang = 0;
    }

    //printf("mem2\n");

    adj_list_elem *tmp;
    for (i = 0; i < n; i++) {
        tmp = adj_lists[i];

        while (tmp) {
            dsu_union(vert[tmp->v], vert[i]);
            tmp = tmp->next;
        }
    }
    //printf("dsu\n");

    int *repr = malloc(n * sizeof(int));
    if (repr == NULL) {
        printf("ERROR\n");
    }
    //printf("mem3\n");

    // for (i = 0; i < n; i++) {
    //     //printf("%d\n", i);
    //     repr[i] = dsu_get(vert[i])->v;
    // }

    // //printf("dsu2\n");

    // int comp_num = 0;
    // for (i = 0; i < n; i++) {
    //     //printf("%d\n", i);
    //     comp_size[comp_num] = 0;
    //     for (j = 0; j < n; j++) {
    //         if (repr[j] == i) {
    //             comp_size[comp_num]++;
    //             comp[j] = comp_num;
    //         }
    //     }
    //     if (comp_size[comp_num]) {
    //         comp_num++;
    //     }
    // }

    // free(repr);
    // //printf("dsu3\n");

    int *reprs = malloc(n * sizeof(int));

    int repr_num = 0;
    for (i = 0; i < n; i++) {
        repr[i] = dsu_get(vert[i])->v;
        for (j = 0; (j < repr_num) && (reprs[j] != repr[i]); j++);
        if (j == repr_num) {
            reprs[j] = repr[i];
            repr_num++;
        }
    }

    for (i = 0; i < repr_num; i++) {
        comp_size[i] = 0;
        for (j = 0; j < n; j++) {
            if (repr[j] == reprs[i]) {
                comp[j] = i;
                comp_size[i]++;
            }
        }
    }

    free(repr);
    free(reprs);

    for (i = 0; i < n; i++) {
        free(vert[i]);
    }

    return repr_num;
}


void add_edge(int v1, int v2, adj_list_elem **adj_lists) {
    adj_list_elem *tmp = malloc(sizeof(adj_list_elem));
    tmp->next = adj_lists[v1];
    tmp->v = v2;
    adj_lists[v1] = tmp;

    tmp = malloc(sizeof(adj_list_elem));
    tmp->next = adj_lists[v2];
    tmp->v = v1;
    adj_lists[v2] = tmp;
}


void pic_to_graph(int w, int h, int **image, int epsilon, adj_list_elem **adj_lists) {
    int i, j;
    //printf("1\n");
    for (i = 1; i < h; i++) {
        for (j = 0; j < w; j++) {
            if (abs(image[i][j] - image[i - 1][j]) < epsilon) {
                add_edge(i * w + j, (i - 1) * w + j, adj_lists);
            }
        }
    }
    //printf("2\n");
    for (i = 0; i < h - 1; i++) {
        for (j = 0; j < w; j++) {
            if (abs(image[i][j] - image[i + 1][j]) < epsilon) {
                add_edge(i * w + j, (i + 1) * w + j, adj_lists);
            }
        }
    }
    //printf("3\n");
    for (i = 0; i < h; i++) {
        for (j = 1; j < w; j++) {
            if (abs(image[i][j] - image[i][j - 1]) < epsilon) {
                add_edge(i * w + j, i * w + (j - 1), adj_lists);
            }
        }
    }
    //printf("4\n");
    for (i = 0; i < h; i++) {
        for (j = 0; j < w - 1; j++) {
            if (abs(image[i][j] - image[i][j + 1]) < epsilon) {
                add_edge(i * w + j, i * w + (j + 1), adj_lists);
            }
        }
    }
}


void clear_graph(int n, adj_list_elem **adj_lists) {
    adj_list_elem *tmp1, *tmp2;
    int i;
    for (i = 0; i < n; i++) {
        //printf("%d\n", i);
        tmp1 = adj_lists[i];
        while (tmp1) {
            tmp2 = tmp1;
            tmp1 = tmp1->next;
            free(tmp2);
        }
    }
}


void read_image(char *filename, unsigned int *width, unsigned int *height, unsigned char **image) {
    unsigned int error;
    error = lodepng_decode32_file(image, width, height, filename);
    if (error) printf("decoder error %u: %s\n", error, lodepng_error_text(error));
}


void image_to_mat(int w, int h, unsigned char *image, int **mat) {
    int i, j, r, g, b;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            //printf("%d %d\n", i, j);
            r = image[(i * w + j) * 4];
            //printf("c\n");
            g = image[(i * w + j) * 4 + 1];
            //printf("d\n");
            b = image[(i * w + j) * 4 + 2];
            //printf("a\n");
            mat[i][j] = (int)(0.2126 * r + 0.7152 * g + 0.0722 * b);
            //printf("b\n");
        }
    }
}


void mat_to_image(int w, int h, int **mat, unsigned char *image) {
    int i, j;
    //unsigned char *image = malloc(4 * w * h * sizeof(unsigned char));
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            image[(i * w + j) * 4] = mat[i][j];
            image[(i * w + j) * 4 + 1] = mat[i][j];
            image[(i * w + j) * 4 + 2] = mat[i][j];
            image[(i * w + j) * 4 + 3] = 255;
        }
    }
}


void write_image(char *filename, unsigned int width, unsigned int height, unsigned char *image) {
    lodepng_encode32_file(filename, image, width, height);
    free(image);
}


void paint_components(int width, int height, int *comp, int comp_num, int *comp_size, int min_size, int **mat, unsigned char *image) {
    int i, j, color_num = 0;
    for (i = 0; i < comp_num; i++) {
        if (comp_size[i] > min_size) {
            color_num++;
        }
    }

    int colors[comp_num];
    j = 0;
    for (i = 0; i < comp_num; i++) {
        if (comp_size[i] > min_size) {
            colors[i] = j;
            j++;
        }
    }

    printf("%d of which are of size more than %d and will be painted.\n", color_num, min_size);


    int r[color_num], g[color_num], b[color_num];
    double h, s, v, c, x, m, rp, gp, bp;
    for (i = 0; i < color_num; i++) {
        h = (360. * i) / (double)color_num;
        s = 1.;
        v = 1.;

        c = v * s;
        x = c * (1 - abs(((int)h / 60) % 2 - 1));
        m = v - c;

        switch ((int)h / 60) {
        case 0:
            rp = c;
            gp = x;
            bp = 0;
            break;
        case 1:
            rp = x;
            gp = c;
            bp = 0;
            break;
        case 2:
            rp = 0;
            gp = c;
            bp = x;
            break;
        case 3:
            rp = 0;
            gp = x;
            bp = c;
            break;
        case 4:
            rp = x;
            gp = 0;
            bp = c;
            break;
        case 5:
            rp = c;
            gp = 0;
            bp = x;
            break;
        default:
            rp = 0;
            gp = 0;
            bp = 0;
            break;
        }

        r[i] = (int)((rp + m) * 255);
        g[i] = (int)((gp + m) * 255);
        b[i] = (int)((bp + m) * 255);
    }

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            if (comp_size[comp[i * width + j]] > min_size) {
                image[(i * width + j) * 4] = r[colors[comp[i * width + j]]];
                image[(i * width + j) * 4 + 1] = g[colors[comp[i * width + j]]];
                image[(i * width + j) * 4 + 2] = b[colors[comp[i * width + j]]];
                image[(i * width + j) * 4 + 3] = 255;
            }
            else {
                image[(i * width + j) * 4] = mat[i][j];
                image[(i * width + j) * 4 + 1] = mat[i][j];
                image[(i * width + j) * 4 + 2] = mat[i][j];
                image[(i * width + j) * 4 + 3] = 255;
            }
        }
    }
}


void gauss_filter(int width, int height, int **pic) {
    int res[height][width], i, j;
    for (i = 1; i < height - 1; i++) {
        for (j = 1; j < width - 1; j++) {
            res[i][j] = 0.084 * pic[i][j]
                      + 0.084 * pic[i + 1][j] + 0.084 * pic[i - 1][j]
                      + 0.084 * pic[i][j + 1] + 0.084 * pic[i][j - 1]
                      + 0.063 * pic[i - 1][j - 1] + 0.063 * pic[i - 1][j + 1]
                      + 0.063 * pic[i + 1][j - 1] + 0.063 * pic[i + 1][j + 1];
        }
    }
    for (i = 1; i < height - 1; i++) {
        for (j = 1; j < width - 1; j++) {
            pic[i][j] = res[i][j];
        }
    }
}


void sobel_filter(int width, int height, int **pic) {
    int res[height][width], i, j, gx, gy;
    for (i = 1; i < height - 1; i++) {
        for (j = 1; j < width - 1; j++) {
            gx = -pic[i - 1][j - 1] + pic[i - 1][j + 1]
                - 2 * pic[i][j - 1] + 2 * pic[i][j + 1]
                - pic[i + 1][j - 1] + pic[i + 1][j + 1];
            gy = pic[i - 1][j - 1] + pic[i - 1][j + 1]
                + 2 * pic[i - 1][j] - 2 * pic[i + 1][j]
                - pic[i + 1][j - 1] - pic[i + 1][j + 1];
            res[i][j] = sqrt(gx * gx + gy * gy);
        }
    }
    for (i = 1; i < height - 1; i++) {
        for (j = 1; j < width - 1; j++) {
            pic[i][j] = res[i][j];
        }
    }
}


int main(int argc, char **argv) {
    // char *in_filename = argc > 1 ? argv[1] : "in_pic.png",
    //            *out_filename = argc > 2 ? argv[2] : "out_pic.png";
    char in_filename[32] = "in_pic.png", out_filename[32] = "out_pic.png";
    int epsilon = 5, min_size = 30;

    int i;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'i':
                    strcpy(in_filename, argv[i + 1]);
                    break;
                case 'o':
                    strcpy(out_filename, argv[i + 1]);
                    break;
                case 'e':
                    epsilon = atoi(argv[i + 1]);
                    break;
                case 's':
                    min_size = atoi(argv[i + 1]);
                    break;
                default:
                    break;
            }
        }
    }

    //printf("%s", in_filename);

    unsigned int width, height;
    unsigned char *image;
    read_image(in_filename, &width, &height, &image);
    
    int **pic = malloc(height * sizeof(int *));
    for (i = 0; i < height; i++) {
        pic[i] = malloc(width * sizeof(int));
        if (pic[i] == NULL) {
            printf("ERROR\n");
        }
    }

    image_to_mat(width, height, image, pic);
    gauss_filter(width, height, pic);
    sobel_filter(width, height, pic);

    adj_list_elem **adj_lists = calloc(width * height, sizeof(adj_list_elem *));
    pic_to_graph(width, height, pic, epsilon, adj_lists);

    int comp[width * height], comp_num, comp_size[width * height];
    comp_num = find_components(width * height, adj_lists, comp, comp_size);
    printf("Found %d components of sizes:\n", comp_num);

    for (i = 0; i < comp_num; i++) {
        if (comp_size[i] > min_size) {
            printf("%d, ", comp_size[i]);
        }
    }
    printf("\n");

    clear_graph(width * height, adj_lists);

    paint_components(width, height, comp, comp_num, comp_size, min_size, pic, image);

    //mat_to_image(width, height, pic, image);
    //printf("m2i\n");

    for (i = 0; i < height; i++) {
        free(pic[i]);
    }
    free(pic);

    write_image(out_filename, width, height, image);
  return 0;
}