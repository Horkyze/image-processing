#define Pxl Pixel *
#define IMAGE IMAGE_S *

typedef struct Pixel {
    byte r,g,b;
}Pixel;

typedef struct IMAGE_S{
    Pxl image;
    int w;
    int h;
}IMAGE_S;

typedef struct Point{
    int x;
    int y;
    int z;
}Point;

Pxl create_buffer(int size){
    Pxl p = (Pxl) malloc(size * sizeof(Pixel));
    if (p == 0){
        printf("[!!] ERROR: failed to allocate space \n");
        exit(-1);
    }
    return p;
}


IMAGE IMG_create_empty(int w, int h){
    int size = w * h;
    IMAGE i = (IMAGE) malloc( sizeof(IMAGE_S));
    i->w = w;
    i->h = h;
    i->image = create_buffer(size);
    return i;
}

IMAGE IMG_load_raw(int w, int h, const char * path){
    IMAGE i = IMG_create_empty(w, h);
    FILE * fd = fopen(path, "rb");
    if (fd == 0) {
        printf("Failed to open file: %s\n", path);
    } else if (w * h * sizeof(Pixel) == fread(i->image, sizeof(Pixel), w*h, fd)){
        printf("Failed to read file, or reading beyond EOF\n");
    }
    return i;
}


Pxl IMG_get_pixel(IMAGE i, int x, int y){
    return (Pxl) (i->image + x + (y * i->w));
}

// a - appaned, w - overwrite
IMAGE IMG_set_pixel(IMAGE i, int x, int y, char mode, int r, int g, int b){
    if (x < 0 || y < 0)
        return 0;
    Pxl pixel = IMG_get_pixel(i, x, y);
    if (mode == 'w'){
        pixel->r = check_overflow(r, 0, 255);
        pixel->g = check_overflow(g, 0, 255);
        pixel->b = check_overflow(b, 0, 255);
    }
    if (mode == 'a'){
        pixel->r = check_overflow(r + pixel->r, 0, 255);
        pixel->g = check_overflow(g + pixel->g, 0, 255);
        pixel->b = check_overflow(b + pixel->b, 0, 255);
    }

}

void IMG_copy_over(IMAGE from, IMAGE to){
    int size = to->w * to-> h * sizeof(Pixel);
    //printf("copy %d bytes from %p into %p\n", size, from->image, to->image);
    memcpy((void *)to->image, (void *)from->image, size);
}

void IMG_apply_kernel(IMAGE image, double ** matrix, int kernel_size){
    int x = 0, y = 0, w = image->w, h = image->h;
    int mx, my, cx, cy;
    double r, g, b;
    IMAGE new_image = IMG_create_empty(w, h);
    for (x = 0; x < w; x++)
    for (y = 0; y < h; y++){
        // new pixel value
        r = 0;
        g = 0;
        b = 0;
        for (mx = 0; mx < kernel_size; mx++)
        for (my = 0; my < kernel_size; my++) {
            cx = (x - kernel_size / 2 + mx + w) % w;
            cy = (y - kernel_size / 2 + my + h) % h;
            //printf("  %d %d\n", cx, cy);
            r += matrix[mx][my] * (IMG_get_pixel(image, cx, cy))->r;
            g += matrix[mx][my] * (IMG_get_pixel(image, cx, cy))->g;
            b += matrix[mx][my] * (IMG_get_pixel(image, cx, cy))->b;
        }
        IMG_set_pixel(new_image, cx, cy, 'w', check_overflow(r, 0, 255), check_overflow(g, 0, 255), check_overflow(b, 0, 255));
    }
    image->image = new_image->image;
}

void IMG_draw_line_bresenham(IMAGE img, int x0, int y0, int x1, int y1)
{
   int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
   int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
   int err = dx-dy, e2, x2;                       /* error value e_xy */
   int ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);

   for ( ; ; ){                                         /* pixel loop */
       IMG_set_pixel(img, x0, y0, 'w', 255*abs(err-dx+dy)/ed, 100, 100);
      e2 = err; x2 = x0;
      if (2*e2 >= -dx) {                                    /* x step */
         if (x0 == x1) break;
         if (e2+dy < ed) IMG_set_pixel(img, x0, y0+ 'w',sy, 255*(e2+dy)/ed, 100, 100);
         err -= dy; x0 += sx;
      }
      if (2*e2 <= dy) {                                     /* y step */
         if (y0 == y1) break;
         if (dx-e2 < ed) IMG_set_pixel(img, x2+ sx, 'w',y0, 255*(dx-e2)/ed, 100, 100);
         err += dx; y0 += sy;
    }
  }
}



Point IMG_get_bezier(Point a, Point b, Point c, Point d, double t) {
    Point P;

    P.x = pow((1 - t), 3) * a.x + 3 * t * pow((1 -t), 2) * b.x + 3 * (1-t) * pow(t, 2)* c.x + pow (t, 3)* d.x;
    P.y = pow((1 - t), 3) * a.y + 3 * t * pow((1 -t), 2) * b.y + 3 * (1-t) * pow(t, 2)* c.y + pow (t, 3)* d.y;
    P.z = pow((1 - t), 3) * a.z + 3 * t * pow((1 -t), 2) * b.z + 3 * (1-t) * pow(t, 2)* c.z + pow (t, 3)* d.z;

    return P;
}

void IMG_draw_curve(IMAGE i, Point a, Point b, Point c, Point d){
    double t, step = 0.0001;
    Point p1, p2;
    for (t = 0; t <= 1; t += step){
        p1 = IMG_get_bezier(a, b, c, d, t);
        p2 = IMG_get_bezier(a, b, c, d, t+step);
        //printf("p1: %d %d\n", p1.x, p1.y);
        //printf("p2: %d %d\n", p2.x, p2.y);
        if(! (p1.x == p2.x && p1.y == p2.y) )
            IMG_draw_line_bresenham(i, p1.x, p1.y, p2.x, p2.y);
        //IMG_set_pixel(i, p.x, p.y, 'w', 255, 255, 255);
    }
}
