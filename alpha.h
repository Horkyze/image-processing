#define APxl A_Pixel *
#define AIMG A_IMAGE_S *

typedef struct A_Pixel {
    byte r,g,b,a;
}A_Pixel;

typedef struct A_IMAGE_S{
    APxl image;
    int w;
    int h;
}A_IMAGE_S;

APxl A_create_buffer(int size){
    APxl p = (APxl) malloc(size * sizeof(A_Pixel));
    if (p == 0){
        printf("[!!] ERROR: failed to allocate space \n");
        exit(-1);
    }
    return p;
}

AIMG A_create_empty(int w, int h){
    int size = w * h;
    AIMG i = (AIMG) malloc( sizeof(A_IMAGE_S));
    i->w = w;
    i->h = h;
    i->image = A_create_buffer(size);
    return i;
}

AIMG A_load_raw(int w, int h, const char * path){
    AIMG i = A_create_empty(w, h);
    FILE * fd = fopen(path, "rb");
    if (fd == 0) {
        printf("Failed to open file: %s\n", path);
    } else if (w * h * sizeof(Pixel) == fread(i->image, sizeof(A_Pixel), w*h, fd)){
        printf("Failed to read file, or reading beyond EOF\n");
    }
    return i;
}

APxl A_get_pixel(AIMG a, int x, int y){
    return (APxl) (a->image + x + (y * a->w));
}

double A_get_alpha(AIMG a, int x, int y){
	APxl p = A_get_pixel(a, x, y);
	return (p->a == 0 )? 0 :(double) (p->a / 255.0);
}

// merge RGBA into RGB from starting position [sx, sy]
void A_merge_RGB_RGBA(IMAGE img, AIMG a, int sx, int sy){
	int x, y;
	int r, g, b;
	double alpha;
	for (y = sy; (y < a->h + sy) && (y < img->h); y++)
	for (x = sx; (x < a->w + sx) && (x < img->w); x++){
	   	alpha = A_get_alpha(a, x - sx, y - sy);
	   	//  a.r * a.a    +      i.r * (1 - a.a)
		r = (int) (A_get_pixel(a, x - sx, y - sy)->r * alpha + IMG_get_pixel(img, x, y)->r * (1 - alpha) );
		g = (int) (A_get_pixel(a, x - sx, y - sy)->g * alpha + IMG_get_pixel(img, x, y)->g * (1 - alpha) );
		b = (int) (A_get_pixel(a, x - sx, y - sy)->b * alpha + IMG_get_pixel(img, x, y)->b * (1 - alpha) );
		IMG_set_pixel(img, x, y, 'w', r, g, b);
	}
}
