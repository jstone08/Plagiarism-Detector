#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>
#include <math.h>


/*
 *     brightness.c
 *     by Jordan Stone, 9/17/16
 *     HW1 - Files, Pictures, and Interfaces
 *
 *     This program reads an image file from the command line. 
 *     Then it computes the average brightness of the image. 
 */ 

/***************************** DECLARATIONS *********************************/
FILE *open_file(int argc, char *argv[]);
void scan_pixels(Pnmrdr_T img);
float compute_average(int sum, int total_pixels);
void find_brightness(double raw_avg, unsigned denominator);


/**************************** IMPLEMENTATION ********************************/
int main(int argc, char *argv[])
{
    FILE *imagef = open_file(argc, argv);

    TRY
        Pnmrdr_T img = Pnmrdr_new(imagef);

        scan_pixels(img);

        EXCEPT(Pnmrdr_Badformat)  /* to test, try passing in .jpeg */
            printf("Input is not correctly formatted\n");
        fclose(imagef);
        exit(EXIT_FAILURE);

        EXCEPT(Pnmrdr_Count)  /* testing if # of pixels is correct */
        printf("Wrong number of pixels read\n");
        fclose(imagef);
        exit(EXIT_FAILURE);

    END_TRY;
    fclose(imagef);

    return EXIT_SUCCESS;
}

/* Name: open_file
 * Arguments: the argument count and the array of arguments
 * Purpose: This function opens the file and takes in a file from standard
 *      input if no file is typed on the command line.
 */
FILE *open_file(int argc, char *argv[])
{
    if (argc == 1) {
        if (stdin == NULL) {    /* checks for no arguments given */
            exit(EXIT_FAILURE);
        }
        else {
            FILE *imagef = stdin; /* takes from standard input */
            return imagef;
        }
    }

        if (argc > 2) {      /* checks for correct number of arguments */
        fprintf(stderr, "ERROR: Too many files\n");
        exit(EXIT_FAILURE); 
    }

    FILE *imagef = fopen(argv[1], "rb");
    return imagef;
}



/* Name: scan_pixels
 * Arguments: the pnm reader
 * Purpose: Scans through all pixels in image file, adds up total brightness
 *      value and gets total # of pixels
 */
void scan_pixels(Pnmrdr_T img)
{
    Pnmrdr_mapdata img_data = Pnmrdr_data(img); 

    int total_pixels = img_data.height * img_data.width;

    unsigned sum_of_numerators = 0;

    for (int i = 0; i < total_pixels; i++) {  /* adds all numerators up */
                sum_of_numerators += Pnmrdr_get(img);
    }

    Pnmrdr_free(&img);     /* frees memory at the address of the image */

    double raw_average = compute_average(sum_of_numerators, total_pixels);

    find_brightness(raw_average, img_data.denominator);
}



/* Name: compute_average
 * Arguments: the sum of all pixel numerators and the total number of pixels
 * Purpose: finds the average numerator of all the pixels
 */
float compute_average(int sum, int total_pixels)
{
    return sum / total_pixels;
}

/* Name: find_brightness
 * Arguments: average of all of the numerators, common denominator of all
 *        pixels
 * Purpose: divides the average numerator by the common denominator and
 *      prints the final average rounded to three decimal places    
 */
void find_brightness(double raw_avg, unsigned denominator)
{
    float final_average = raw_avg / denominator;

    /* multiplies by 1000 b/c round() only rounds to nearest integer */
    printf("%0.3f\n", round(final_average*1000)/1000);
}
