clear

PRODUCT=nixie
FOLDER=vclock

echo
echo "********************** Build $PRODUCT *************************"

QTDIR=/home/$USER/Qt5.8.0/5.8/gcc_64
DEVEL=/home/$USER/Documents/Development
OUTPUT=$DEVEL/$FOLDER/$PRODUCT/install/linux64/Output
TARGET=$OUTPUT/debian/usr/bin/$PRODUCT
BINARIES=/home/$USER/Documents/Development/build-$FOLDER-Desktop_Qt_5_8_0_GCC_64bit-Release
VERSION=0.0.0

if [ -d $TARGET ]
then
echo Remove $TARGET
rm -r $TARGET
fi

if [ -f $OUTPUT/$PRODUCT-$VERSION.deb ]
then
echo Remove $OUTPUT/$PRODUCT-$VERSION.deb
chmod +w $OUTPUT/$PRODUCT-$VERSION.deb
rm $OUTPUT/$PRODUCT-$VERSION.deb
fi

if [ -f $OUTPUT/$PRODUCT-$VERSION.zip ]
then
echo Remove $OUTPUT/$PRODUCT-$VERSION.zip
chmod +w $OUTPUT/$PRODUCT-$VERSION.zip
rm $OUTPUT/$PRODUCT-$VERSION.zip
fi

mkdir "$TARGET"

echo Copy binary files
cp $BINARIES/$PRODUCT/$PRODUCT $TARGET/$PRODUCT
cp ../../icons/*48.png $TARGET
cp $PRODUCT-deb.run $TARGET/$PRODUCT.run

echo Copy plugins files
mkdir $TARGET/plugins
cp $BINARIES/$PRODUCT/plugins/*.so $TARGET/plugins

echo Copy translation files
mkdir $TARGET/languages
cp ../../../languages/*_??.qm $TARGET/languages

echo Copy .destktop file
cp $OUTPUT/*.desktop $TARGET

echo ************* Stls
mkdir $TARGET/stl
cp ../../stl/*.stl "$TARGET/stl"

echo ************* Images
mkdir $TARGET/images
cp ../../images/about.png $TARGET/images
cp ../../images/table.jpg $TARGET/images
cp ../../images/bkgnd.jpg $TARGET/images

echo ************* ringtones
mkdir $TARGET/ringtones
cp ../../ringtones/*.* $TARGET/ringtones

echo Copy Qt plugins
mkdir $TARGET/imageformats
cp $QTDIR/plugins/imageformats/*.* $TARGET/imageformats
mkdir $TARGET/bearer
cp $QTDIR/plugins/bearer/*.* $TARGET/bearer
mkdir $TARGET/platforms
cp $QTDIR/plugins/platforms/*.* $TARGET/platforms
mkdir $TARGET/audio
cp $QTDIR/plugins/audio/*.* $TARGET/audio
mkdir $TARGET/mediaservice
cp $QTDIR/plugins/mediaservice/*.* $TARGET/mediaservice
mkdir $TARGET/xcbglintegrations
cp $QTDIR/plugins/xcbglintegrations/*.* $TARGET/xcbglintegrations

echo Copy Qt binary files
mkdir $TARGET/lib
cp $QTDIR/lib/libicudata.so.56 $TARGET/lib
cp $QTDIR/lib/libicui18n.so.56 $TARGET/lib
cp $QTDIR/lib/libicuuc.so.56 $TARGET/lib/libicuuc.so.56
cp $QTDIR/lib/libQt5Core.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5DBus.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5Gui.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5Network.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5Widgets.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5XcbQpa.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5XcbQpa.so.5.8.0 $TARGET/lib
cp $QTDIR/lib/libQt5Xml.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5Multimedia.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5MultimediaWidgets.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5MultimediaWidgets.so.5.8.0 $TARGET/lib
cp $QTDIR/lib/libqgsttools_p.so.1 $TARGET/lib
cp $QTDIR/lib/libQt5OpenGL.so.5 $TARGET/lib
cp $QTDIR/lib/libqgsttools_p.so.1.0.0 $TARGET/lib
cp $QTDIR/lib/libQt5OpenGL.so.5.8.0 $TARGET/lib
cp $QTDIR/lib/libQt5DBus.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5DBus.so.5.8.0  $TARGET/lib
cp $QTDIR/bin/linguist $TARGET

echo Build debian package
cd Output
chmod 755 debian/DEBIAN/post*
chmod 755 debian/DEBIAN/pre*
dpkg-deb --build debian $PRODUCT-linux64-x86-$VERSION.deb
cd ..

echo Build zip file
cp $PRODUCT-zip.run $TARGET/$PRODUCT.run
cd $TARGET
cd ..
zip  -q -r -9 $OUTPUT/$PRODUCT-linux64-x86-$VERSION.zip nixie
echo "********************** End build *************************"


