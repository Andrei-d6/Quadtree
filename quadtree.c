#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct QuadtreeNode {
	unsigned char blue,green,red;
	unsigned int area;
	int top_left, top_right;
	int bottom_left, bottom_right;
}__attribute__((packed)) QuadtreeNode;

typedef struct pixel{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
}pixel;

typedef struct nod{
	unsigned char red, green, blue;
	unsigned int area;
	struct nod *top_left;
	struct nod *top_right;
	struct nod *bottom_right;
	struct nod *bottom_left;
}nod;


//functie pentru obtinerea unui radical dintr-o putere a lui 2
int radical(int x)
{
	int sqrt = 2;

	while(sqrt * sqrt != x)
		sqrt = sqrt *2;

	return sqrt;

}

// eliberare de memorie pentru o matrice
void freeMatrix(pixel **grid, int height)
{
	int i;
	for(i = 0; i < height; i++)
	{
		free(grid[i]);
	}
	free(grid);
}

// adaugare de nod intr-un arbore cuaternar
void addNode(nod **cap, pixel p, int l)
{
	nod *q = malloc(sizeof(nod));
	q->red = p.red;
	q->green = p.green;
	q->blue = p.blue;
	q->area = l*l;
	q->top_left = NULL;
	q->top_right = NULL;
	q->bottom_right = NULL;
	q->bottom_left = NULL;

	if( *cap == NULL )
	{
		*cap = q;
		return;
	}
	else
	{
		if((*cap)->top_left == NULL)
		{
			(*cap)->top_left = q;
		}
		else
		if((*cap)->top_right == NULL)
		{
			(*cap)->top_right = q;
		}
		else
		if((*cap)->bottom_right == NULL)
		{
			(*cap)->bottom_right = q;
		}
		else
		if((*cap)->bottom_left == NULL)
		{
			(*cap)->bottom_left = q;
		}
	}
}

// calcularea gradului de uniformitate a unei zone din matrice
// si a culorii medii
int check_if_uniform(pixel **grid,int x,int y, int l, pixel *p)
{
	long long mean = 0;
	int i,j;
	long long red,green,blue;
	long long r,g,b;
	long long size;
	size = l;
	red = 0;
	green = 0;
	blue =0; 	
	r = g = b =0;

	for(i = x; i < x+l; i++)
		for(j = y; j < y+l; j++)
		{	
			r= grid[i][j].red;
			red = red + r;
			
			g = grid[i][j].green;
			green = green + g;
			
			b = grid[i][j].blue;
			blue = blue + b;	
		}
		
		red = red/(size*size);
		green = green/(size*size);
		blue = blue/(size*size);	


		(*p).red = red;
		(*p).green = green;
		(*p).blue = blue;
	
	for(i = x; i < x+l; i++)
		for(j = y; j < y+l; j++)
			{
				mean += (red - grid[i][j].red)*(red - grid[i][j].red);
				mean += (blue - grid[i][j].blue)*(blue - grid[i][j].blue);
				mean += (green - grid[i][j].green)*(green - grid[i][j].green);
			}
	
	mean = mean/(3*l*l);
	return mean;

}

// crearea unei imagini pornind de la o matrice de rgb-uri
void copyImage(pixel **grid, int width, int height,int max_num, char *file_name)
{
	FILE *f = fopen(file_name,"wb");

	fprintf(f, "%s\n", "P6");
	fprintf(f, "%d %d\n", height, width);
    fprintf(f, "%d\n", max_num);

	
    int i,j;
  
	for(i = 0; i < height; i++)
	 	fwrite(grid[i],sizeof(pixel),width,f);

	fclose(f);
}

/* compresia intr-un arbore cuaternar a unei matrici de rgb-uri */
void compress(nod **cap, int x,int y,int l,int width,pixel **grid,int prag,int *nr_of_colors, int *nr_of_nodes)
{	
	// avg => nivelul actual de uniformitate
	int avg;
	//p => culoarea medie pentru patratul actual
	pixel p;
	avg = check_if_uniform(grid,x,y,l,&p);
	
	if(l < 1 )
		return;

	(*nr_of_nodes) = (*nr_of_nodes)+1;
	addNode(cap,p,l);

	if(avg > prag)
	{
	
	compress(&(*cap)->top_left, x, y, l/2, width, grid,prag, nr_of_colors, nr_of_nodes);
	(*nr_of_colors) = (*nr_of_colors)+1;
	compress(&(*cap)->top_right, x, y + l/2, l/2, width, grid,prag, nr_of_colors, nr_of_nodes);
	(*nr_of_colors) = (*nr_of_colors)+1;
	compress(&(*cap)->bottom_right, x + l/2, y + l/2, l/2, width, grid,prag, nr_of_colors, nr_of_nodes);
	(*nr_of_colors) = (*nr_of_colors)+1;
	compress(&(*cap)->bottom_left, x + l/2, y, l/2, width, grid, prag, nr_of_colors, nr_of_nodes);	
	(*nr_of_colors) = (*nr_of_colors)+1;

	}	
}

/* aducerea unui arbore cuaternar la o forma in care
   fiecare nivel contine fix 4^nivel noduri
   radacina se considera pe nivelul 0 in acest caz*/
void complete(nod **cap, int l, int depth, int line) 
{	
	if(*cap == NULL)
		return;

	if(line < depth && (*cap)->top_left == NULL)
	{	
		pixel p;
		p.red = (*cap)->red;
		p.green = (*cap)->green;
		p.blue = (*cap)->blue;

		addNode(cap,p,l);
		addNode(cap,p,l);
		addNode(cap,p,l);
		addNode(cap,p,l);
	}

	complete(&(*cap)->top_left,l/2,depth,line+1);
	complete(&(*cap)->top_right,l/2,depth,line+1);
	complete(&(*cap)->bottom_right,l/2,depth,line+1);
	complete(&(*cap)->bottom_left,l/2,depth,line+1);
}

/* determinarea adancimii unui arbore cuaternar */
int Depth(nod* cap)
{
    if (cap == NULL)
        return 0;
    else
    {
       
        int lf = Depth(cap->top_left);
        int lr = Depth(cap->top_right);
        int br = Depth(cap->bottom_right);
        int bl = Depth(cap->bottom_left);
 
       
    	int max = lf;
    	if(lr > max)
    		max = lr;
    	if(br > max)
    		max = br;
    	if(bl > max)
    		max = bl;
    	return(max+1);
    }
}

// functie pentru incarcarea unui vector de tip QuadtreeNode
// parcurgand pe nivele un arbore 
void loadGivenLevel(QuadtreeNode *v, int *k,int *son_nr, nod *cap, int level)
{
    if (cap == NULL)
        return;
   
    if (level == 1)
        {	
        	v[*k].red = cap->red;
        	v[*k].green = cap->green;
        	v[*k].blue = cap->blue;
        	v[*k].area = cap->area;

        	if(cap->top_left == NULL)
        	{
        		v[*k].top_left = -1;
        		v[*k].top_right = -1;
        		v[*k].bottom_right = -1;
        		v[*k].bottom_left = -1;
        	}
        	else
        	{	
        		v[*k].top_left = *son_nr;
        		*son_nr = (*son_nr)+1;
        		v[*k].top_right = *son_nr;
        		*son_nr = (*son_nr)+1;
        		v[*k].bottom_right = *son_nr;
        		*son_nr = (*son_nr)+1;
        		v[*k].bottom_left = *son_nr;
        		*son_nr = (*son_nr)+1;
        	}
        	*k = (*k)+1;
        }
    else if (level > 1)
    {
        loadGivenLevel(v, k, son_nr, cap->top_left, level-1);
        loadGivenLevel(v, k, son_nr, cap->top_right, level-1);
        loadGivenLevel(v, k, son_nr, cap->bottom_right, level-1);
        loadGivenLevel(v, k, son_nr, cap->bottom_left, level-1);
    }
}
 
/* crearea unui fisier de compresie pornind de la un vector dat*/
void outputFile(QuadtreeNode *v, int *k,int *son_nr, nod *cap, int depth, char *file_name, int nr_of_colors, int nr_of_nodes, int width)
{
	int i,j;	

	for(i = 1; i <= depth; i++)
		loadGivenLevel(v, k, son_nr, cap, i);

	FILE *f = fopen(file_name,"wb");
	
	fwrite(&nr_of_colors,sizeof(int),1,f);
	fwrite(&nr_of_nodes,sizeof(int),1,f);
	
	
	for(i = 0; i < *k; i++)
	{	
		fwrite(&v[i],sizeof(QuadtreeNode),1,f);
	}
		
	fclose(f);

}

/* Citirea unui vector dintr-un fisier binar */
void readVector(QuadtreeNode *v, int *nr_of_colors,int *nr_of_nodes, char *file_name)
{
	FILE *f = fopen(file_name,"rb");

	fread(nr_of_colors,sizeof(int),1,f);
	fread(nr_of_nodes,sizeof(int),1,f);

	int i;

	for(i = 0; i < *nr_of_nodes; i++)
	{
		fread(&v[i].red,sizeof(unsigned char),1,f);
		fread(&v[i].green,sizeof(unsigned char),1,f);
		fread(&v[i].blue,sizeof(unsigned char),1,f);
		fread(&v[i].area,sizeof(int),1,f);
		fread(&v[i].top_left,sizeof(int),1,f);
		fread(&v[i].top_right,sizeof(int),1,f);
		fread(&v[i].bottom_left,sizeof(int),1,f);
		fread(&v[i].bottom_right,sizeof(int),1,f);
		
	}
}

/* incarca pe un patrat din matrice un rgb dat*/
void gridValues(pixel **grid, int x, int y, int l,unsigned char red, unsigned char green, unsigned char blue)
{
	int i,j;
	for(i = x; i < x+l; i++)
		for(j = y; j < y+l; j++)
			{
				grid[i][j].red = red;
				grid[i][j].green = green;
				grid[i][j].blue = blue;
			}
}

/* Incarca un vector intr-un arbore cuaternar*/
void loadQuadTree(nod **cap,QuadtreeNode *v,int l,int i, int nr_of_nodes)
{
	if(nr_of_nodes == 0 || l < 1)
	  return;

	pixel p;
	p.red = v[i].red;
	p.green = v[i].green;
	p.blue = v[i].blue;

	addNode(cap,p,l);

	if(v[i].top_left + v[i].top_right + v[i].bottom_right + v[i].bottom_left != -4 )
	{
	loadQuadTree(&(*cap)->top_left,v,l/2,v[i].top_left,nr_of_nodes-1);
	loadQuadTree(&(*cap)->top_right,v,l/2,v[i].top_right, nr_of_nodes-1);
	loadQuadTree(&(*cap)->bottom_right,v,l/2,v[i].bottom_right, nr_of_nodes-1);
	loadQuadTree(&(*cap)->bottom_left,v,l/2,v[i].bottom_left, nr_of_nodes-1);
	}	
}

/* Incarcarea cu valori a unei matrici pornind de la un arbore */
void loadGrid(nod *cap, pixel **grid, int x, int y, int l, int *nr_of_colors, int nr_of_nodes)
{	
		
	if((*nr_of_colors) == 0 || l == 0 )
		return;

	 if(cap->top_left == NULL)
	 {	 
	 	gridValues(grid,x,y,l,cap->red,cap->green,cap->blue); 
	 	*nr_of_colors = (*nr_of_colors)-1;
	 }
	 else
	 {	
	 	
	 	loadGrid(cap->top_left, grid, x, y, l/2, nr_of_colors,nr_of_nodes);
	 	loadGrid(cap->top_right, grid, x, y+l/2, l/2,nr_of_colors,nr_of_nodes);
	 	loadGrid(cap->bottom_left, grid, x+l/2, y, l/2,nr_of_colors,nr_of_nodes);
		loadGrid(cap->bottom_right, grid, x+l/2, y+l/2,l/2,nr_of_colors,nr_of_nodes);
	 	
	 }
}

/* Incarcarea unei matrici cu "mediile" culorilor de pe frunzele 
   a doi arbori cuaternari cu acelasi numar de nivele si noduri */
void loadGridBonus(nod *cap1, nod *cap2, pixel **grid, int x, int y, int l)
{	
		
	if( l == 0 )
		return;

	 if(cap1->top_left == NULL)
	 {	 
	 	gridValues(grid,x,y,l,(cap1->red+cap2->red)/2,(cap1->green + cap2->green)/2,(cap1->blue + cap2->blue)/2); 
	 }
	 else
	 {	
	 	
	 	loadGridBonus(cap1->top_left, cap2->top_left, grid, x, y, l/2);
	 	loadGridBonus(cap1->top_right, cap2->top_right, grid, x, y+l/2, l/2);
	 	loadGridBonus(cap1->bottom_left, cap2->bottom_left, grid, x+l/2, y, l/2);
		loadGridBonus(cap1->bottom_right, cap2->bottom_right,grid, x+l/2, y+l/2,l/2);
	 	
	 }
}

/* Eliberare de memorie pentru un arbore */
void freeQuadTree(nod *cap)
{
	if(cap == NULL)
		return;

	freeQuadTree(cap->top_left);
	freeQuadTree(cap->top_right);
	freeQuadTree(cap->bottom_right);
	freeQuadTree(cap->bottom_left);

	free(cap);
	cap = NULL;

}

/* Oglindirea unei imagini pe orizontala */
void flipH(nod **cap)
{
	if(*cap == NULL)
		return;

	if((*cap)->top_left != NULL)
	{
		nod *aux = NULL;
		//swap top_left cu top_right
		aux = (*cap)->top_left;
		(*cap)->top_left = (*cap)->top_right;
		(*cap)->top_right = aux;

		//swap bottom_left cu bottom_right
		aux = (*cap)->bottom_left;
		(*cap)->bottom_left = (*cap)->bottom_right;
		(*cap)->bottom_right = aux;

		flipH(&(*cap)->top_left);
		flipH(&(*cap)->top_right);
		flipH(&(*cap)->bottom_right);
		flipH(&(*cap)->bottom_left);

	}
}
/* Oglindirea unei imagini pe verticala */
void flipV(nod **cap)
{
	if(*cap == NULL)
		return;

	if((*cap)->top_left != NULL)
	{	
		nod *aux = NULL;
		//swap top_left cu bottom_left
		aux = (*cap)->top_left;
		(*cap)->top_left = (*cap)->bottom_left;
		(*cap)->bottom_left = aux;

		//swap top_right cu bottom_right
		aux = (*cap)->top_right;
		(*cap)->top_right = (*cap)->bottom_right;
		(*cap)->bottom_right = aux;

		
		flipV(&(*cap)->top_left);
		flipV(&(*cap)->top_right);
		flipV(&(*cap)->bottom_right);
		flipV(&(*cap)->bottom_left);	
	}
}

int main(int argc, char *argv[])
{	
	
	int width, height;
	int max_num;
	int i,j;
	int prag;
		
	/* CERINTA 1 */
	if(strcmp(argv[1],"-c") == 0)
	{
	pixel **grid;
	FILE *f = fopen(argv[3],"rb");
	char type[3];
	
	prag = atoi(argv[2]);

	fscanf(f,"%2s",type);
	type[2]='\0';
	fscanf(f, "%d", &height);
    fscanf(f, "%d", &width);
    fscanf(f, "%d", &max_num);
    char c;
    fscanf(f, "%c ",&c);
  
    //alocare de memorie pentru matricea de culori
    grid = malloc(height*sizeof(pixel*));

	for(i = 0; i < height; i++)
		grid[i] = malloc(width*sizeof(pixel));

	//citirea culorilor efective
	for(i = 0; i < height; i++)
		for(j = 0; j < width; j++)
			{fread(&grid[i][j].red,sizeof(unsigned char),1,f);
			 fread(&grid[i][j].green,sizeof(unsigned char),1,f);
			 fread(&grid[i][j].blue,sizeof(unsigned char),1,f);}

	fclose(f);	

	//in cap se va retine radacina arborelui cuaternar
	nod *cap;	
	cap = NULL;

	int nr_of_colors,nr_of_nodes,depth; 
	nr_of_colors = 0;
	nr_of_nodes = 0;
	depth = 0;
	
	compress(&cap,0,0,width,width,grid,prag,&nr_of_colors,&nr_of_nodes);
	depth = Depth(cap);

	QuadtreeNode *v;

	v = malloc(nr_of_nodes*sizeof(QuadtreeNode));
	int k = 0, son_nr = 1;

	outputFile(v,&k,&son_nr,cap,depth+1,argv[4],nr_of_colors,nr_of_nodes,width);

	/* eliberarea memoriei alocate cerintei 1*/
	freeMatrix(grid,width);
	if(v != NULL)
	free(v);
	freeQuadTree(cap);
	}
	
	/* CERINTA 2*/
	if(strcmp(argv[1],"-d") == 0)
	{	
		pixel **grid;
		QuadtreeNode *v;
		int max_num = 255;
		int nr_of_colors,nr_of_nodes,width;

		FILE *f = fopen(argv[2],"rb");


		fread(&nr_of_colors,sizeof(int),1,f);
		fread(&nr_of_nodes,sizeof(int),1,f);
		v = malloc(nr_of_nodes*sizeof(QuadtreeNode));
		fclose(f);
		
		readVector(v,&nr_of_colors, &nr_of_nodes,argv[2]);

		if(nr_of_colors == 0 && nr_of_nodes == 1)
			nr_of_colors = 1;
		
		width = radical(v[0].area);
	
   	 	grid = malloc(width*sizeof(pixel*));

		for(i = 0; i < width; i++)
			grid[i] = malloc(width*sizeof(pixel));

		int k = 0;
		int x,y,l;
		x = y = 0;
		l = width;

		nod *tree;	
		tree = NULL;
		
		loadQuadTree(&tree,v,width,k,nr_of_nodes);

		x = y =0;
		l = width;

		if(nr_of_nodes == 1)
		gridValues(grid,x,y,l,v[0].red,v[0].green,v[0].blue);
	else
		loadGrid(tree,grid,x,y,l,&nr_of_colors,nr_of_nodes);

		copyImage(grid,width,width,max_num,argv[3]);
		
		/* eliberarea memoriei alocate cerintei 2*/
		freeMatrix(grid,width);
		if(v != NULL)
		free(v);
		freeQuadTree(tree);
	}
	

	/*CERINTA 3*/
	if(strcmp(argv[1],"-m") == 0)
	{	
		pixel **grid;
		int rotire;
		if(strcmp(argv[2],"h")==0)
			rotire = 1; 
		else if(strcmp(argv[2],"v")==0)
			rotire = 2;
		else
			rotire = 0;
		//rotire == 1 => rotire orizontala
		//rotire == 2=> rotire verticala

	prag = atoi(argv[3]);
	FILE *f = fopen(argv[4],"rb");
	char type[3];
	
	fscanf(f,"%2s",type);
	type[2]='\0';
	fscanf(f, "%d", &height);
    fscanf(f, "%d", &width);
    fscanf(f, "%d", &max_num);
    char c;
    fscanf(f,"%c",&c);
   
    grid = malloc(height*sizeof(pixel*));

	for(i = 0; i < height; i++)
		grid[i] = malloc(width*sizeof(pixel));

	for(i = 0; i < height; i++)
		for(j = 0; j < width; j++)
			fread(&grid[i][j],sizeof(pixel),1,f);

	fclose(f);

	nod *cap = NULL;

	int nr_of_colors,nr_of_nodes,depth; 
	nr_of_colors = 0;
	nr_of_nodes = 0;
	depth = 0;
	
	int x,y,l;

	compress(&cap,0,0,width,width,grid,prag,&nr_of_colors,&nr_of_nodes);

	depth = Depth(cap);

	if(rotire == 1)
	{
		flipH(&cap);
	}
	else if (rotire == 2)
	{
		flipV(&cap);
	}

	x = y =0;
	l = width;

	loadGrid(cap,grid,x,y,l,&nr_of_colors,nr_of_nodes);
	
	copyImage(grid,width,width,max_num,argv[5]);
	
	/* eliberarea memoriei alocate cerintei 3*/
	freeMatrix(grid,width);
	freeQuadTree(cap);
	}


	/* Bonus */
	if(strcmp(argv[1],"-o") == 0)
	{
		nod *cap1 = NULL;
		nod *cap2 = NULL;



		pixel **grid1,**grid2;
		int width1, height1;
		int width2, height2;

	prag = atoi(argv[2]);
	FILE *f = fopen(argv[3],"rb");
	char type[3];
	
	fscanf(f,"%2s",type);
	type[2]='\0';
	fscanf(f, "%d", &height1);
    fscanf(f, "%d", &width1);
    fscanf(f, "%d", &max_num);
    char c;
    fscanf(f,"%c",&c);
   
    grid1 = malloc(height1*sizeof(pixel*));

	for(i = 0; i < height1; i++)
		grid1[i] = malloc(width1*sizeof(pixel));

	for(i = 0; i < height1; i++)
		for(j = 0; j < width1; j++)
			fread(&grid1[i][j],sizeof(pixel),1,f);

	fclose(f);


	FILE *g = fopen(argv[4],"rb");
	
	
	fscanf(g,"%2s",type);
	type[2]='\0';
	fscanf(g, "%d", &height2);
    fscanf(g, "%d", &width2);
    fscanf(g, "%d", &max_num);
    fscanf(g,"%c",&c);
   
    grid2 = malloc(height2*sizeof(pixel*));

	for(i = 0; i < height2; i++)
		grid2[i] = malloc(width2*sizeof(pixel));

	for(i = 0; i < height2; i++)
		for(j = 0; j < width2; j++)
			fread(&grid2[i][j],sizeof(pixel),1,g);

	fclose(g);



	int nr_of_colors1,nr_of_nodes1,depth1; 
	int nr_of_colors2,nr_of_nodes2,depth2;
	nr_of_colors1 = 0;
	nr_of_nodes1 = 0;
	nr_of_colors2 = 0;
	nr_of_nodes2 = 0;
	depth1 = 0;
	depth2 = 0;
	
	int x,y,l;

	x = 0;
	y = 0;
	l = width1;

	compress(&cap1,0,0,width1,width1,grid1,prag,&nr_of_colors1,&nr_of_nodes1);
	compress(&cap2,0,0,width2,width2,grid2,prag,&nr_of_colors2,&nr_of_nodes2);

	depth1 = Depth(cap1);
	depth2 = Depth(cap2);

	int depth;

	if(depth1 > depth2)
		depth = depth1;
	else
		depth = depth2;

	

	int k = 1;
	printf("%d %d\n",depth1,depth2 );

	complete(&cap1,width1, depth,1);
	complete(&cap2,width2, depth,1);

	depth1 = Depth(cap1);
	depth2 = Depth(cap2);

	printf("%d %d\n",depth1,depth2 );

	loadGridBonus(cap1,cap2,grid1 ,x,y,l);
	copyImage(grid1,width1,width1,max_num,argv[5]);

	freeMatrix(grid1,width1);
	freeQuadTree(cap1);
	freeMatrix(grid2,width2);
	freeQuadTree(cap2);

	}
	return 0;
}