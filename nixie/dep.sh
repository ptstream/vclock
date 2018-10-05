echo "Copy dependencies"

cp $1/config/nixie.ini $2

if [ ! -d "$2/stl" ]
then
mkdir $2/stl
fi

if [ ! -d "$2/images" ]
then
mkdir $2/images
fi

if [ ! -d "$2/diaporama" ]
then
mkdir $2/diaporama
fi

if [ ! -d "$2/languages" ]
then
mkdir $2/languages
fi

if [ ! -d "$2/ringtones" ]
then
mkdir $2/ringtones
fi

cp $1/stl/*.stl $2/stl
cp $1/images/*.* $2/images
cp $1/diaporama/*.* $2/diaporama
cp $1/ringtones/*.* $2/ringtones
cp $1/../languages/*.qm $2/languages
