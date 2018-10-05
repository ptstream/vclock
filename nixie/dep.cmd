echo "Copy dependencies"
copy  %1\config\nixie.ini %2
xcopy /I /Y %1\stl\*.stl %2\stl
xcopy /I /Y %1\images\*.* %2\images
xcopy /I /Y %1\diaporama\*.* %2\diaporama
xcopy /I /Y %1\ringtones\*.* %2\ringtones
xcopy /I /Y %1\..\languages\*.qm %2\languages
