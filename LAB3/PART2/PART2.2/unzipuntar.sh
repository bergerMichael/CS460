echo Enter .tgz file:

read $tarfile

zcat $tarfile | tar xvf -