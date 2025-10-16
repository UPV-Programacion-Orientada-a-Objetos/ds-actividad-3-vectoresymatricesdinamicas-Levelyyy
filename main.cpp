#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <new>
using namespace std;

const int prod = 5;
const int matp = 4;
const int semIni = 4;

float** crearMat(int f,int c){
    float** mm = nullptr;
    try{
        mm = new float*[f];
        for(int i=0;i<f;i++){
            mm[i] = new float[c];
            for(int j=0;j<c;j++){
                mm[i][j] = 0;
            }
        }
    }catch(bad_alloc &err){
        if(mm){
            for(int i=0;i<f;i++) delete[] mm[i];
            delete[] mm;
        }
        mm = nullptr;
    }
    return mm;
}

void borrarMat(float** &m,int f){
    if(!m) return;
    for(int i=0;i<f;i++){
        delete[] m[i];
    }
    delete[] m;
    m = nullptr;
}

bool masSemana(float** &m,int f,int &c){
    int nuevo = c+1;
    float** temp = nullptr;
    try{
        temp = new float*[f];
        for(int i=0;i<f;i++){
            temp[i] = new float[nuevo];
        }
        for(int i=0;i<f;i++){
            for(int j=0;j<c;j++){
                temp[i][j] = m[i][j];
            }
            temp[i][c] = 0;
        }
    }catch(bad_alloc &e){
        if(temp){
            for(int i=0;i<f;i++) delete[] temp[i];
            delete[] temp;
        }
        return false;
    }
    borrarMat(m,f);
    m = temp;
    c = nuevo;
    return true;
}

string leerLinea(){
    string x;
    getline(cin,x);
    return x;
}

int leerInt(string txt,int minv,int maxv){
    while(true){
        cout<<txt;
        string s = leerLinea();
        try{
            int num = stoi(s);
            if(num<minv || num>maxv){
                cout<<"Valor fuera de rango ["<<minv<<"-"<<maxv<<"]. Intenta de nuevo.\n";
                continue;
            }
            return num;
        }catch(...){
            cout<<"Entrada invalida, intenta otra vez.\n";
        }
    }
}

float leerFloat(string txt){
    while(true){
        cout<<txt;
        string s = leerLinea();
        try{
            float v = stof(s);
            if(v<0){
                cout<<"No puede ser negativo.\n";
                continue;
            }
            return v;
        }catch(...){
            cout<<"Dato invalido.\n";
        }
    }
}

void verPlan(float** m,int f,int c){
    cout<<"\nPlan de Produccion ("<<f<<" Productos x "<<c<<" Semanas)\n";
    cout<<setw(8)<<"Prod";
    for(int j=0;j<c;j++) cout<<setw(10)<<"S"<<j;
    cout<<"\n";
    for(int i=0;i<f;i++){
        cout<<setw(8)<<"P"<<i;
        for(int j=0;j<c;j++){
            cout<<setw(10)<<fixed<<setprecision(0)<<m[i][j];
        }
        cout<<"\n";
    }
}

void consumoSem(float** plan,int f,int c,int sem,const int req[prod][matp],float *out){
    for(int k=0;k<matp;k++) out[k]=0;
    for(int i=0;i<f;i++){
        float unidades = plan[i][sem];
        for(int k=0;k<matp;k++){
            out[k] += unidades * (float)req[i][k];
        }
    }
}

float calcCOGS(float** plan,int f,int sem,const float *costos){
    float t=0;
    for(int i=0;i<f;i++){
        t += plan[i][sem] * costos[i];
    }
    return t;
}

float invFinal(const float *invIni,const float *precio,const float *cons,float *resta){
    float val=0;
    for(int k=0;k<matp;k++){
        float rem = invIni[k] - cons[k];
        if(rem<0) rem = 0;
        resta[k] = rem;
        val += rem * precio[k];
    }
    return val;
}

int main(){
    const int req[prod][matp] = {
        {2,0,1,0},
        {1,1,0,2},
        {0,2,2,0},
        {3,0,1,1},
        {0,1,0,3}
    };

    float *cost = nullptr;
    float *inv = nullptr;
    try{
        cost = new float[prod];
        inv = new float[matp];
    }catch(...){
        cout<<"Error al asignar memoria\n";
        if(cost) delete[] cost;
        if(inv) delete[] inv;
        return 1;
    }

    cost[0]=5; cost[1]=8; cost[2]=6; cost[3]=10; cost[4]=7.5;
    inv[0]=1000; inv[1]=800; inv[2]=600; inv[3]=400;
    const float precioMat[matp] = {2,3.5,1.5,4};

    int f=prod;
    int c=semIni;
    float **plan = crearMat(f,c);
    if(!plan){
        delete[] cost; delete[] inv;
        cout<<"Error al crear matriz inicial\n";
        return 1;
    }

    for(int i=0;i<f;i++){
        for(int j=0;j<c;j++){
            plan[i][j] = 100 + i*60 + j*50;
        }
    }

    cout<<"--- Sistema de Planificacion y Costos (COGS) ---\n\n";
    cout<<"Inicializacion exitosa. Matriz de Planificacion: "<<f<<" Productos x "<<c<<" Semanas.\n";

    bool sigue=true;
    while(sigue){
        cout<<"\n--- Menu Principal ---\n";
        cout<<"1. Ver Plan de Produccion\n";
        cout<<"2. Agregar Nueva Semana (Redimensionar Matriz)\n";
        cout<<"3. Modificar Produccion\n";
        cout<<"4. Calcular COGS y Final Inventory\n";
        cout<<"5. Salir\n";

        int op = leerInt("Opcion seleccionada: ",1,5);

        if(op==1){
            verPlan(plan,f,c);
        }else if(op==2){
            cout<<"Agregando Semana "<<(c+1)<<"...\n";
            bool ok = masSemana(plan,f,c);
            if(ok){
                cout<<"Matriz redimensionada a "<<f<<"x"<<c<<". ¡Memoria gestionada con exito!\n";
            }else{
                cout<<"Error al redimensionar.\n";
            }
        }else if(op==3){
            int p = leerInt("Ingrese Producto (0-4): ",0,f-1);
            int s = leerInt("Ingrese Semana (0-"+to_string(c-1)+"): ",0,c-1);
            float q = leerFloat("Cantidad a producir: ");
            plan[p][s] = q;
            cout<<"Produccion actualizada.\n";
        }else if(op==4){
            int s = leerInt("Ingrese Semana para el calculo de costos: ",0,c-1);
            float totU=0;
            for(int i=0;i<f;i++) totU += plan[i][s];

            float cons[matp];
            consumoSem(plan,f,c,s,req,cons);

            float cogs = calcCOGS(plan,f,s,cost);
            float rest[matp];
            float invVal = invFinal(inv,precioMat,cons,rest);

            cout<<"\n--- Reporte de Costos (Semana "<<s<<") ---\n";
            cout<<"Produccion Total (Semana "<<s<<"): "<<fixed<<setprecision(0)<<totU<<" unidades.\n";
            cout<<"Costo Total de Produccion (COGS): $"<<fixed<<setprecision(2)<<cogs<<"\n";
            cout<<"Consumo de Materia Prima: ";
            for(int k=0;k<matp;k++){
                cout<<"MP-0"<<(k+1)<<" ("<<fixed<<setprecision(0)<<cons[k]<<" uni)";
                if(k<matp-1) cout<<", ";
            }
            cout<<".\n";
            cout<<"Valor del Inventario Final (M.P. restante): $"<<fixed<<setprecision(2)<<invVal<<"\n";
        }else if(op==5){
            cout<<"Liberando memoria de Matriz y Vectores Dinamicos...\n";
            sigue=false;
        }
    }

    borrarMat(plan,f);
    delete[] cost;
    delete[] inv;
    cout<<"Sistema cerrado.\n";
    return 0;
}


