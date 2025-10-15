#include <iostream>
#include <iomanip>
#include <limits>
#include <new>
using namespace std;

const int numProd = 5;
const int numMP = 4;
const int semanasIni = 4;

float** crearMatriz(int filas, int cols){
    float **m;
    m = new float*[filas];
    for(int i=0;i<filas;i++){
        m[i]=new float[cols];
        for(int j=0;j<cols;j++){
            m[i][j]=0;
        }
    }
    return m;
}

void borrarMatriz(float **&m, int filas){
    if(!m) return;
    for(int i=0;i<filas;i++) delete[] m[i];
    delete[] m;
    m=nullptr;
}

bool addSemana(float **&m, int filas, int &cols){
    int nuevo = cols+1;
    float **tmp = new float*[filas];
    for(int i=0;i<filas;i++) tmp[i]=new float[nuevo];

    for(int i=0;i<filas;i++){
        for(int j=0;j<cols;j++){
            tmp[i][j] = m[i][j];
        }
        tmp[i][cols]=0;
    }
    borrarMatriz(m,filas);
    m = tmp;
    cols = nuevo;
    return true;
}

void mostrarPlan(float **m, int f, int c){
    cout<<"\nPlan actual ("<<f<<" productos x "<<c<<" semanas)\n";
    cout<<setw(8)<<"Prod";
    for(int j=0;j<c;j++) cout<<setw(10)<<"S"<<j;
    cout<<"\n";
    for(int i=0;i<f;i++){
        cout<<setw(8)<<"P"<<i;
        for(int j=0;j<c;j++) cout<<setw(10)<<m[i][j];
        cout<<"\n";
    }
}

void limpiar(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
}

int leerInt(string t, int min, int max){
    int x;
    cout<<t;
    while(true){
        if(!(cin>>x)){
            cout<<"dato invalido:";
            limpiar();
            continue;
        }
        if(x<min || x>max){
            cout<<"fuera de rango:";
            continue;
        }
        limpiar();
        return x;
    }
}

float leerFloat(string t){
    float x;
    cout<<t;
    while(true){
        if(!(cin>>x)){
            cout<<"dato invalido:";
            limpiar();
            continue;
        }
        if(x<0){
            cout<<"no negativo:";
            continue;
        }
        limpiar();
        return x;
    }
}

void consumoSemana(float **mat, int f, int c, int sem, const int req[numProd][numMP], float *out){
    for(int k=0;k<numMP;k++) out[k]=0;
    for(int i=0;i<f;i++){
        float unidades = mat[i][sem];
        for(int k=0;k<numMP;k++){
            out[k]+= unidades * req[i][k];
        }
    }
}

float calcCosto(float **mat, int f, int c, int sem, const float *cost){
    float tot=0;
    for(int i=0;i<f;i++) tot+=mat[i][sem]*cost[i];
    return tot;
}

float inventarioFinal(const float *ini, const float *costo, const float *usado, float *queda){
    float val=0;
    for(int k=0;k<numMP;k++){
        float rest = ini[k]-usado[k];
        if(rest<0) rest=0;
        queda[k]=rest;
        val+=rest*costo[k];
    }
    return val;
}

int main(){
    const int req[numProd][numMP]={
        {2,0,1,0},
        {1,1,0,2},
        {0,2,2,0},
        {3,0,1,1},
        {0,1,0,3}
    };

    float *costoProd = new float[numProd];
    float *invMP = new float[numMP];
    const float costoMP[numMP] = {2.2,3.4,1.6,4.1};

    costoProd[0]=5.5; costoProd[1]=8; costoProd[2]=6.2; costoProd[3]=10.3; costoProd[4]=7.1;
    invMP[0]=1000; invMP[1]=800; invMP[2]=600; invMP[3]=500;

    int filas=numProd;
    int cols=semanasIni;
    float **plan = crearMatriz(filas,cols);

    // inicializo algunos datos medio al azar
    for(int i=0;i<filas;i++){
        for(int j=0;j<cols;j++){
            plan[i][j] = 200 + (i*50) + (j*30);
        }
    }

    cout<<"--- Sistema simple de COGS ---\n";
    cout<<"Datos cargados ("<<filas<<"x"<<cols<<")\n";

    bool seguir=true;
    while(seguir){
        cout<<"\n1.Ver plan\n2.Agregar semana\n3.Modificar prod\n4.Calcular COGS\n5.Reporte MP\n6.Salir\nOpcion:";
        int op;
        if(!(cin>>op)){limpiar();continue;}
        limpiar();

        if(op==1){
            mostrarPlan(plan,filas,cols);
        }
        else if(op==2){
            cout<<"Agregando semana nueva...\n";
            addSemana(plan,filas,cols);
            cout<<"ok ahora "<<cols<<" semanas.\n";
        }
        else if(op==3){
            int p = leerInt("producto (0-4): ",0,filas-1);
            int s = leerInt("semana (0-"+to_string(cols-1)+"): ",0,cols-1);
            float q = leerFloat("cantidad: ");
            plan[p][s]=q;
            cout<<"Actualizado!\n";
        }
        else if(op==4){
            int s = leerInt("Semana para calculo: ",0,cols-1);
            float total=0;
            for(int i=0;i<filas;i++) total+=plan[i][s];
            float cogs = calcCosto(plan,filas,cols,s,costoProd);
            float usado[numMP];
            consumoSemana(plan,filas,cols,s,req,usado);
            float queda[numMP];
            float valor = inventarioFinal(invMP,costoMP,usado,queda);

            cout<<fixed<<setprecision(2);
            cout<<"--- Semana "<<s<<" ---\n";
            cout<<"Prod total: "<<total<<"  COGS: $"<<cogs<<"\n";
            cout<<"MP usada:\n";
            for(int k=0;k<numMP;k++) cout<<" MP"<<k<<": "<<usado[k]<<"\n";
            cout<<"Inventario final (valor): $"<<valor<<"\n";
        }
        else if(op==5){
            int s = leerInt("Semana: ",0,cols-1);
            float usado[numMP];
            consumoSemana(plan,filas,cols,s,req,usado);
            cout<<"Consumo MP semana "<<s<<":\n";
            for(int k=0;k<numMP;k++) cout<<" MP"<<k<<": "<<usado[k]<<"\n";
        }
        else if(op==6){
            cout<<"Saliendo...\n";
            seguir=false;
        }
        else{
            cout<<"Opcion incorrecta\n";
        }
    }

    borrarMatriz(plan,filas);
    delete[] costoProd;
    delete[] invMP;
    cout<<"Fin del programa\n";
    return 0;
}

