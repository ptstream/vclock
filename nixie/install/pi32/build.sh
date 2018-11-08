clear

CWD=$PWD

PRODUCT=nixie
FOLDER=vclock

echo
echo "********************** Build $PRODUCT *************************"

QTDIR=/usr/lib/arm-linux-gnueabihf/qt5
DEVEL=/home/pi/Devel
OUTPUT=$DEVEL/$FOLDER/$PRODUCT/install/pi32/Output
TARGET=$OUTPUT/debian/usr/bin/$PRODUCT
BINARIES=/home/pi/Devel/build-$FOLDER-Desktop-Release
VERSION=0.0.2

if [ -d $TARGET ]
then
echo Remove $TARGET
rm -r $TARGET
fi

if [ -f $OUTPUT/$PRODUCT-$VERSION.deb ]
then
echo "Remove $OUTPUT/$PRODUCT-$VERSION.deb"
chmod +w $OUTPUT/$PRODUCT-$VERSION.deb
rm $OUTPUT/$PRODUCT-$VERSION.deb
fi

if [ -f $OUTPUT/$PRODUCT-$VERSION.zip ]
then
echo "Remove $OUTPUT/$PRODUCT-$VERSION.zip"
chmod +w $OUTPUT/$PRODUCT-$VERSION.zip
rm $OUTPUT/$PRODUCT-$VERSION.zip
fi

mkdir "$TARGET"

echo "Copy binary files"
cp $BINARIES/$PRODUCT/$PRODUCT $TARGET/$PRODUCT
chmod +x $TARGET/$PRODUCT
cp ../../icons/*48.png $TARGET
cp $PRODUCT $TARGET/$PRODUCT.run
chmod u+rwx $TARGET/$PRODUCT.run

echo "Copy ini file"
cp ../../config/$PRODUCT.ini $TARGET/$PRODUCT.ini

echo "Copy plugins files"
mkdir $TARGET/plugins
cp $BINARIES/$PRODUCT/plugins/*.so $TARGET/plugins

echo "Copy translation files"
mkdir $TARGET/languages
cp ../../../languages/*_??.qm $TARGET/languages

echo "Copy .destktop file"
cp $OUTPUT/*.desktop $TARGET

echo "Copy Stls"
mkdir $TARGET/stl
cp ../../stl/*.stl $TARGET/stl

echo "Copy Images"
mkdir $TARGET/images
cp ../../images/about.png $TARGET/images
cp ../../images/table.jpg $TARGET/images
cp ../../images/bkgnd.jpg $TARGET/images

echo "Copy ringtones"
mkdir $TARGET/ringtones
cp ../../ringtones/*.* $TARGET/ringtones

echo "Copy diaporama"
mkdir $TARGET/diaporama
cp ../../diaporama/*.* $TARGET/diaporama

echo "Copy Qt Images plugins"
mkdir $TARGET/imageformats
cp $QTDIR/plugins/imageformats/*.* $TARGET/imageformats

echo "Copy Qt Bearer plugins"
mkdir $TARGET/bearer
cp $QTDIR/plugins/bearer/*.* $TARGET/bearer

echo "Copy Qt platforms"
mkdir $TARGET/platforms
cp $QTDIR/plugins/platforms/*.* $TARGET/platforms

echo "Build rapbian package"
cd $CWD
cd Output
chmod 755 debian/DEBIAN/post*
chmod 755 debian/DEBIAN/pre*
dpkg-deb --build debian $PRODUCT-rpi3-$VERSION.deb
cd ..
echo "********************** End build *************************"


