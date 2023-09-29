#!/bin/sh

LOCAL_ARCH=`$SX/vasy/arch`

if [ "$MALLOC_DEBUG" = "1" ]
then

if [ $LOCAL_ARCH = macOS -o $LOCAL_ARCH = mac86 -o $LOCAL_ARCH = mac64 ]
then
    
   # On positionne des variables pour rendre malloc() et free() plus stricts
   # (voir page man de malloc sous MacOS X)

   MallocScribble=1
   export MallocScribble

   MallocPreScribble=1
   export MallocPreScribble

   MallocGuardEdges=1
   export MallocGuardEdges

   MallocStackLogging=1
   export MallocStackLogging

   MallocCheckHeapEach=1000
   export MallocCheckHeapEach

   MallocCheckHeapStart=1000
   export MallocCheckHeapStart

   MallocGuardEdges=1
   export MallocGuardEdges

   # création d'un fichier contenant les messages emis par malloc() et free()
   MallocLogFile=/tmp/malloc_errors
   export MallocLogFile 

fi

fi
