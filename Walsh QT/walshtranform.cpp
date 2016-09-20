#include "walshtranform.h"
#include <QFile>
#include <QDebug>
#include <QObject>

WalshTransform::WalshTransform()
{
    size = 0;
    size_aligned = 0;
    walsh = NULL; //w[i][j] == w_i(x), where j < x < j + 1
    image = NULL;
    spectr = NULL;
    spectr2d = NULL;
    imageRestored = NULL;

    log = NULL;
}

void WalshTransform::setLog(QTextEdit *textEdit)
{
    log = textEdit;
}

void WalshTransform::doLog(QString s)
{
//    if (log == NULL)
        qDebug() << s;
 //   else
        log->append(s);
}

WalshTransform::~WalshTransform()
{
    clearAll();
}

void WalshTransform::clearAll()
{
    deleteMatrix(size_aligned, walsh);
    deleteMatrix(size_aligned, image);
    deleteMatrix(size_aligned, spectr);
    deleteMatrix(size_aligned, spectr2d);
    deleteMatrix(size_aligned, imageRestored);

    walsh = NULL;
    image = NULL;
    spectr = NULL;
    spectr2d = NULL;
    imageRestored = NULL;


    size = 0;
    size_aligned = 0;
}

int** WalshTransform::newMatrix(int n, int ** z)
{
    if (z != NULL)
        deleteMatrix(n, z);
    z = new int* [n];
    for (int i = 0; i < n; i++)
    {
        z[i] = new int[n];
        memset(z[i], 0, n * sizeof(int));
    }
    return z;
}

void WalshTransform::deleteMatrix(int n, int ** z)
{
    if (z == NULL)
        return;
    for (int i = 0; i < n; i++)
        delete [] (z[i]);
    delete [] z;
}

void WalshTransform::generate(int n, int **w)
{
    int n2 = n >> 1;

    for (int x = 0; x < n; x++)
        w[0][x] = 1;

    for (int i = 1; i < n; i++)
    {
        for (int x = 0; x < n2; x++)
            w[i][x] = w[i/2][2*x];

        //even i
        if (i % 2 == 0)
        {
            for (int x = n2; x < n; x++)
                w[i][x] = w[i][x-n2];
        }
        //odd i
        else
        {
            for (int x = n2; x < n; x++)
                w[i][x] = - w[i][x-n2];
        }
    }
    doLog("Walsh base function generated." );
    doLog("Size: " + QString::number(n) + ".");
}

void WalshTransform::print(int n, int **w)
{
    QString s;
    for (int i = 0; i < n; i++)
    {
        for (int x = 0; x < n; x++)
             s += QString::number(w[i][x]) + '\t';
        s += '\n';
    }
    s += '\n';
//    doLog(s);

//
//    QFile file("output.log");
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
 //       return;
 //   QTextStream out(&file);
 //   out << size << "\n";
 //   out << s;
}

void WalshTransform::makeFourierWalshInRow(int n, int **w, int **signal, int **spectr)
{
    for (int row = 0; row < n; row++)
        for (int col = 0; col < n; col++)
        {
            spectr[row][col] = 0;
            for (int i = 0; i < n; i++)
                spectr[row][col] += signal[row][i] * w[col][i];
        }
}

void WalshTransform::makeFourierWalshInCol(int n, int **w, int **signal, int **spectr)
{
    for (int col = 0; col < n; col++)
        for (int row = 0; row < n; row++)
        {
            spectr[row][col] = 0;
            for (int i = 0; i < n; i++)
                spectr[row][col] += signal[i][col] * w[row][i];
        }
}

int WalshTransform::align_down(int n)
{
    int k = 0;
    if ((n & (n - 1)) == 0)
        return n;
    while (n >>= 1)
        k++;
    return 1 << k;
}

int WalshTransform::align_up(int n)
{
    int k = 0;
    if ((n & (n - 1)) == 0)
        return n;
    while (n >>= 1)
        k++;
    return 1 << (k + 1);
}

int WalshTransform::load(QString fname)
{
    //read matrix from file "image.txt"
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0;

    clearAll();

    QTextStream in(&file);
    in >> size;
    size_aligned = align_up(size);

    image = newMatrix(size_aligned, image);
    for (int row = 0; row < size; row++)
        for (int col = 0; col < size; col++)
            in >> image[row][col];
    print(size, image);
    doLog("Text file '" + fname + "' loaded.");
    doLog("Size: " + QString::number(size) + ".");

    //prepare Walsh-base function
    walsh = newMatrix(size_aligned, walsh);
    generate(size_aligned, walsh);
//	print(size_aligned, walsh);
    return size_aligned;
}

int WalshTransform::load(QImage *img)
{
    clearAll();

    //read matrix from file "image.txt"
    size = qMax(img->width(), img->height());
    size_aligned = align_up(size);

    image = newMatrix(size_aligned, image);
    for (int row = 0; row < img->height(); row++)
        for (int col = 0; col < img->width(); col++)
            image[row][col] = QColor::fromRgb(img->pixel(col, row)).lightness();
//    print(size, image);
    doLog("Image file loaded.");
    doLog("Size: " + QString::number(img->width()) + 'x' + QString::number(img->height()) + ".");

    //prepare Walsh-base function
    walsh = newMatrix(size_aligned, walsh);
    generate(size_aligned, walsh);
//	print(size_aligned, walsh);
    return size_aligned;
}


void WalshTransform::transform()
{

    //make Fourier
    spectr = newMatrix(size_aligned, spectr);
    spectr2d = newMatrix(size_aligned, spectr2d);

    makeFourierWalshInCol(size_aligned, walsh, image, spectr);

    makeFourierWalshInRow(size_aligned, walsh, spectr, spectr2d);

    doLog("Walsh transform done.");
    print(size_aligned, spectr2d);
}

void WalshTransform::retransform()
{
    //make Fourier
    spectr = newMatrix(size_aligned, spectr);
    imageRestored = newMatrix(size_aligned, imageRestored);

    makeFourierWalshInRow(size_aligned, walsh, spectr2d, spectr);
    makeFourierWalshInCol(size_aligned, walsh, spectr, imageRestored);

    for (int row = 0; row < size_aligned; row++)
        for (int col = 0; col < size_aligned; col++)
            imageRestored[row][col] /= size_aligned;


    doLog("Walsh transform restore image.");
}

QImage* WalshTransform::txtToPixmap(QImage *img, int ARRAY)
{
    int ** a = NULL;
    int n = 0;
    int blueAndRed = 0;

    if (ARRAY == IMAGE_ORIGINAL)
    {
        a = image;
        n = size_aligned;
    }
    else if (ARRAY == SPECTR)
    {
        a = spectr;
        n = size_aligned;
    }
    else if (ARRAY == IMAGE_SPECTR2D)
    {
        a = spectr2d;
        n = size_aligned;
        blueAndRed = 1;
    }
    else if (ARRAY == IMAGE_RESTORED)
    {
        a = imageRestored;
        n = size_aligned;
    }

    if (n > 0)
    {

        if (img != NULL)
            delete img;
        img = new QImage(n, n, QImage::Format_ARGB32);

        int max = 0;
        for (int i = 1; i < n; i++)
            for (int j = 1; j < n; j++)
            {
                if (max < qAbs(a[i][j]) || max == 0)
                    max = qAbs(a[i][j]);
            }
        if (max == 0)
            max = 1;

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
            {
                int t = 255 * a[i][j] / max;
                if (blueAndRed)
                    t = 32 * 255 * a[i][j] / max;

                if (t > 255)
                    t = 255;
                if (t < 0)
                    t = 0;

                if (blueAndRed)
                    img->setPixel(j, i,  QColor(255-t, 255-t, 255).rgb());
                else
                    img->setPixel(j, i,  QColor(t, t, t).rgb());

            }
    }
    return img;
}

int WalshTransform::power2round(int x)
{
    int ans = 1;
    while (x)
    {
        ans <<= 1;
        x >>= 1;
    }
    return ans;
}

void WalshTransform::filter(int level, int filterType)
{
    if (spectr2d)
    {
        long long int sumYES = 0, sumNO = 0;
        int counterYES = 0, counterNO = 0;
        for (int i = 0; i < size_aligned; i++)
        {
            for (int j = 0; j < size_aligned; j++)
                if ((filterType == CROSSDOWN && power2round(i) * power2round(j) > level) ||
                    (filterType == CROSSUP && power2round(i) * power2round(j) > 2 * level) ||
                    (filterType == HYPER && i * j >= level))
                {
                    sumNO += abs(spectr2d[i][j]);
                    counterNO++;
                    spectr2d[i][j] = 0;
                }
                else
                {
                    counterYES++;
                    sumYES += abs(spectr2d[i][j]);
                }
        }
        doLog("Number of afterfilter spectr elements: " + QString::number(counterYES) + " of " + QString::number(counterYES + counterNO));
        doLog("Percent of afterfilter spectr elements: " + QString::number(100 * counterYES / (counterYES + counterNO)) + "%");
        doLog("Percent of afterfilter intensity: " + QString::number(100 * sumYES / (sumYES + sumNO)) + "%");
    }
}
