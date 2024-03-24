struct matrices {
        int row1;
        int col1;
        int a[100];

        int row2;
        int col2;
        int b[100];
};

struct matrix {
        int row;
        int col;
        int a[100];
};

struct matrixInverse {
    int row;
    int col;
    float a[100];
    int flag;
};

program MATRIX_OPERATIONS_PROG {
    version MATRIX_OPERATIONS_VERS {
        matrix MATRIX_ADD(matrices) = 1;
        matrix MATRIX_MULTIPLY(matrices) = 2;
        matrixInverse MATRIX_INVERSE(matrix) = 3;
        matrix MATRIX_TRANSPOSE(matrix) = 4;
    } = 1;
} = 0x23452222;
