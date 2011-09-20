int nextfree=0x300000;

//Proceso Malloc
void* Malloc (int size)
{	
/*	void* zona= (void*)0x300000;
	void* temp;
	//esta parte normaliza el size, NADA MAS.
	if (size%KERNEL_MALLOC_SIZE!=0)
		size=size- size%KERNEL_MALLOC_SIZE + KERNEL_MALLOC_SIZE;


	while (!(  *((int*)zona)==0 || *((int*)zona) - (int)zona - *((int*)(zona+sizeof(int*)))-8 >= size))
	{
		zona=(void*)*((int*)zona);
	}

	if(*((int*)zona)==0)
	{
		temp=zona;
		*((int*)temp)=size+8+(int)temp;
		*((int*)(temp+8+size))=0;
	}
	else
	{
		temp=zona+8+*((int*)(zona+4));
		*(int*)temp=*(int*)zona;
		*((int*)zona)=(int)temp;
	}
	*((int*)(temp+sizeof(int*)))=size;

	return temp+2*sizeof(int*);*/
	void* temp = (void*)nextfree;
	nextfree=nextfree+size;
	return temp;
}

// Malloc inicializado en 0
void* Calloc (int size)
{
	char* temp;
	int i;
	temp=(char*)Malloc(size);
	for(i=0;i<size;i++)
		temp[i]=0;
	return (void*)temp;
}

//Malloc para zonas de video, inicializa en {0007}
void* CallocVid (int size)
{
	char* temp;
	int i;
	temp=(char*)Malloc(size);
	for(i=0;i<size;i+=2)
	{
		temp[i]=0;
		temp[i+1]=0x07;
	}
	return (void*)temp;
}
