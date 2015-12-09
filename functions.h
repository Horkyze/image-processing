#ifndef byte
#define byte unsigned char
#endif

float lerp(float v0, float v1, float t)	{	
	return	v0*(1-t) + v1*t;	
}

byte check_overflow(int a, int min, int max){
    if(a < min)
        a = min;
    else if (a > max)
        a = max;
    return (byte) a;
}

double ** create_2d_array(int rows, int cols, int def){
    double **x = (double **) malloc(rows * sizeof(**x));
    int i, k;
    for (i = 0; i < rows; i++){
        x[i] = (double *)malloc(cols * sizeof(*x));
        for (k = 0; k < cols; k++) {
            x[i][k] = def;
        }
    }
    return x;
}

double ** load_kernel(const char * file, int * size){
    int i, k, value;
    double **kernel,f;
    FILE * fd = fopen(file, "r");
    if(fd == 0){
        printf("Failed to open file: %s\n", file);
        exit(-1);
    }
    fscanf(fd, "%d", size);
    kernel = create_2d_array(*size, *size, 0);
    for(i = 0; i < *size; i++){
        for(k = 0; k < *size; k++){
            fscanf(fd, "%lf", &kernel[i][k]);
            printf("%lf ", kernel[i][k]);
        }
        printf("\n");
    }
    fclose(fd);
    return kernel;
}