#include <stdio.h>
#include <stdint.h>
#include <unistd.h>


static int pBytes[MAXDEG];

static int synBytes[MAXDEG];

static int Lambda[MAXDEG];

static int Omega[MAXDEG];

static int Locs[256];

static int NLocs;

static void copy_poly(int dst[], int src[]) {

    int i;

    for (i = 0; i < MAXDEG; i++)
        dst[i] = src[i];
}

static void zero_poly(int poly[]) {

    int i;

    for (i = 0; i < MAXDEG; i++)
        poly[i] = 0;
}

static void mult_polys(int dst[], int p1[], int p2[]) {

    int i, j;
    int tmp1[MAXDEG * 2];


    for (i = 0; i < (MAXDEG * 2); i++)
        dst[i] = 0;

    for (i = 0; i < MAXDEG; i++) {

        for (j = MAXDEG; j < (MAXDEG * 2); j++)
            tmp1[j] = 0;

        for (j = 0; j < MAXDEG; j++)
            tmp1[j] = GMU(p2[j], p1[i]);

        for (j = (MAXDEG * 2) - 1; j >= i; j--)
            tmp1[j] = tmp1[j - i];

        for (j = 0; j < i; j++)
            tmp1[j] = 0;

        for (j = 0; j < (MAXDEG * 2); j++)
            dst[j] ^= tmp1[j];
    }
}

static void mul_z_poly(int src[]) {

    int i;

    for (i = MAXDEG - 1; i > 0; i--)
        src[i] = src[i - 1];

    src[0] = 0;
}

/* Given Psi compute the evaluator polynomial */
static void compute_omega() {

    int i, product[MAXDEG * 2];

    mult_polys(product, Lambda, synBytes);
    zero_poly(Omega);

    for (i = 0; i < NPAR; i++)
        Omega[i] = product[i];

}

static int compute_discrepancy(int lambda[], int S[], int L, int n) {

    int i, sum = 0;

    for (i = 0; i <= L; i++)
        sum ^= GMU(lambda[i], S[n - i]);

    return sum;
}

static void berlekam(void) {

    int n, L, L2, k, d, i;

    int psi[MAXDEG], psi2[MAXDEG], D[MAXDEG];
    int gamma[MAXDEG];


/* initialize Gamma, */
    zero_poly(gamma);
    gamma[0] = 1;

/* initialize to z */
    copy_poly(D, gamma);
    mul_z_poly(D);

    copy_poly(psi, gamma);

    k = -1;
    L = 0;


    for (n = 0; n < NPAR; n++) {

        d = compute_discrepancy(psi, synBytes, L, n);

        if (d != 0) {

            /* psi2 = psi - d*D */
            for (i = 0; i < MAXDEG; i++)
                psi2[i] = psi[i] ^ GMU(d, D[i]);


            if (L < (n - k)) {

                L2 = n - k;
                k = n - L;

                for (i = 0; i < MAXDEG; i++)
                    D[i] = GMU(psi[i], GIN(d));

                L = L2;
            }

            /* psi = psi2 */
            for (i = 0; i < MAXDEG; i++)
                psi[i] = psi2[i];
        }

        mul_z_poly(D);

    }

    for (i = 0; i < MAXDEG; i++)
        Lambda[i] = psi[i];

    compute_omega();

}

/* Evaluating Lambda at successive values of alpha.  */
static void getlambda(void) {

    int sum, r, k;

    NLocs = 0;

    for (r = 1; r < 256; r++) {

        sum = 0;

        /* evaluate lambda at r */
        for (k = 0; k < NPAR + 1; k++)
            sum ^= GMU(GXE[MOD8(k * r)], Lambda[k]);

        if (sum == 0) {
            Locs[NLocs] = (255 - r);
            NLocs++;
        }
    }
}

static int evaluate_omega(uint8_t codeword[], int csize) {

    int r, i, j, err;


    berlekam();
    getlambda();


    if ((NLocs <= NPAR) && NLocs > 0) {

        /* first check for illegal error locs */
        for (r = 0; r < NLocs; r++) {

            if (Locs[r] >= csize)
                return 0;
        }

        for (r = 0; r < NLocs; r++) {

            int num, denom;

            i = Locs[r];

            /* evaluate Omega at alpha^(-i) */

            num = 0;
            for (j = 0; j < MAXDEG; j++)
                num ^= GMU(Omega[j], GXE[MOD8((255 - i) * j)]);

            /* evaluate Lambda' at alpha^(-i) */
            denom = 0;
            for (j = 1; j < MAXDEG; j += 2)
                denom ^= GMU(Lambda[j], GXE[MOD8((255 - i) * (j - 1))]);

            err = GMU(num, GIN(denom));
            codeword[csize - i - 1] ^= err;

        }

        return 1;

    }

    else
        return 0;
}

