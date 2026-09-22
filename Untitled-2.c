#include <stdio.h>

#define TAM_MEMORIA 100
#define MINIMO -9999
#define MAXIMO 9998

void cargarPrograma(int memory[]);
void ejecutarPrograma(int memory[], int *accumulator, int *instructionCounter,
                      int *instructionRegister, int *operationCode, int *operand);
void vaciadoMemoria(int memory[], int accumulator, int instructionCounter,
                    int instructionRegister, int operationCode, int operand);

int main()
{
    int memory[TAM_MEMORIA];

    int accumulator = 0;
    int instructionCounter = 0;
    int instructionRegister = 0;
    int operationCode = 0;
    int operand = 0;

    int i;

    for (i = 0; i < TAM_MEMORIA; i++)
    {
        memory[i] = 0;
    }

    printf("*** Bienvenido a Simpletron ***\n");
    printf("*** Por favor introduzca su programa una instruccion a la vez ***\n");
    printf("*** Cada instruccion debe ser un numero de 4 digitos ***\n");
    printf("*** Introduzca -9999 para numeros negativos ***\n");
    printf("*** Para terminar de cargar introduzca 9999 ***\n\n");

    cargarPrograma(memory);

    printf("\n*** Se termino de cargar el programa ***\n");
    printf("*** Comienza la ejecucion de Simpletron ***\n\n");

    ejecutarPrograma(memory, &accumulator, &instructionCounter,
                     &instructionRegister, &operationCode, &operand);

    vaciadoMemoria(memory, accumulator, instructionCounter,
                   instructionRegister, operationCode, operand);

    return 0;
}

/* Funcion para cargar el programa en memoria */
void cargarPrograma(int memory[])
{
    int contador = 0;
    int valor;

    while (contador < TAM_MEMORIA)
    {
        printf("%02d ? ", contador);
        scanf("%d", &valor);

        if (valor == 9999)
        {
            break;
        }

        if (valor >= MINIMO && valor <= MAXIMO)
        {
            memory[contador] = valor;
            contador++;
        }
        else
        {
            printf("Valor invalido. Debe estar entre -9999 y 9998.\n");
        }
    }

    if (contador == TAM_MEMORIA)
    {
        printf("\nLa memoria esta llena.\n");
    }
}

/* Funcion principal de ejecucion */
void ejecutarPrograma(int memory[], int *accumulator, int *instructionCounter,
                      int *instructionRegister, int *operationCode, int *operand)
{
    int ejecutando = 1;
    int resultado;

    while (ejecutando)
    {
        /* Buscar la siguiente instruccion */
        if (*instructionCounter < 0 || *instructionCounter >= TAM_MEMORIA)
        {
            printf("\n*** ERROR FATAL ***\n");
            printf("El contador de instrucciones esta fuera de memoria.\n");
            return;
        }

        *instructionRegister = memory[*instructionCounter];

        /* Separar codigo de operacion y operando */
        *operationCode = *instructionRegister / 100;
        *operand = *instructionRegister % 100;

        /* Incrementar antes de ejecutar.
           Las bifurcaciones modificaran este valor cuando sea necesario. */
        (*instructionCounter)++;

        switch (*operationCode)
        {
            /* 10 - Leer */
            case 10:
                printf("Ingrese un numero: ");
                scanf("%d", &resultado);

                if (resultado < MINIMO || resultado > MAXIMO)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("El valor introducido esta fuera de rango.\n");
                    return;
                }

                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("El operando esta fuera de memoria.\n");
                    return;
                }

                memory[*operand] = resultado;
                break;

            /* 11 - Escribir */
            case 11:
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("El operando esta fuera de memoria.\n");
                    return;
                }

                printf("Salida: %d\n", memory[*operand]);
                break;

            /* 20 - Cargar */
            case 20:
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("El operando esta fuera de memoria.\n");
                    return;
                }

                *accumulator = memory[*operand];
                break;

            /* 21 - Almacenar */
            case 21:
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("El operando esta fuera de memoria.\n");
                    return;
                }

                memory[*operand] = *accumulator;
                break;

            /* 30 - Sumar */
            case 30:
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("El operando esta fuera de memoria.\n");
                    return;
                }

                resultado = *accumulator + memory[*operand];

                if (resultado < MINIMO || resultado > 9999)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("Desbordamiento del acumulador.\n");
                    return;
                }

                *accumulator = resultado;
                break;

            /* 31 - Restar */
            case 31:
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("El operando esta fuera de memoria.\n");
                    return;
                }

                resultado = *accumulator - memory[*operand];

                if (resultado < MINIMO || resultado > 9999)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("Desbordamiento del acumulador.\n");
                    return;
                }

                *accumulator = resultado;
                break;

            /* 32 - Dividir */
            case 32:
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("El operando esta fuera de memoria.\n");
                    return;
                }

                if (memory[*operand] == 0)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("Intento de dividir entre cero.\n");
                    return;
                }

                *accumulator = *accumulator / memory[*operand];
                break;

            /* 33 - Multiplicar */
            case 33:
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("El operando esta fuera de memoria.\n");
                    return;
                }

                resultado = *accumulator * memory[*operand];

                if (resultado < MINIMO || resultado > 9999)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("Desbordamiento del acumulador.\n");
                    return;
                }

                *accumulator = resultado;
                break;

            /* 40 - Bifurcar */
            case 40:
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("La bifurcacion apunta fuera de memoria.\n");
                    return;
                }

                *instructionCounter = *operand;
                break;

            /* 41 - Bifurcar si es negativo */
            case 41:
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("La bifurcacion apunta fuera de memoria.\n");
                    return;
                }

                if (*accumulator < 0)
                {
                    *instructionCounter = *operand;
                }

                break;

            /* 42 - Bifurcar si es cero */
            case 42:
                if (*operand < 0 || *operand >= TAM_MEMORIA)
                {
                    printf("\n*** ERROR FATAL ***\n");
                    printf("La bifurcacion apunta fuera de memoria.\n");
                    return;
                }

                if (*accumulator == 0)
                {
                    *instructionCounter = *operand;
                }

                break;

            /* 43 - Alto */
            case 43:
                ejecutando = 0;
                break;

            /* Codigo invalido */
            default:
                printf("\n*** ERROR FATAL ***\n");
                printf("Codigo de operacion invalido: %d\n", *operationCode);
                return;
        }
    }

    printf("\n*** Termino la ejecucion de Simpletron ***\n");
}

/* Funcion para mostrar los registros y toda la memoria */
void vaciadoMemoria(int memory[], int accumulator, int instructionCounter,
                    int instructionRegister, int operationCode, int operand)
{
    int i;

    printf("\n");
    printf("============================================\n");
    printf("           VACIADO DE SIMPLETRON\n");
    printf("============================================\n");

    printf("REGISTROS:\n");
    printf("Acumulador              %+05d\n", accumulator);
    printf("Contador de instrucciones %02d\n", instructionCounter);
    printf("Registro de instruccion %+05d\n", instructionRegister);
    printf("Codigo de operacion      %02d\n", operationCode);
    printf("Operando                 %02d\n", operand);

    printf("\nMEMORIA:\n");

    printf("        ");
    for (i = 0; i < 10; i++)
    {
        printf("%6d", i);
    }

    printf("\n");

    for (i = 0; i < TAM_MEMORIA; i++)
    {
        if (i % 10 == 0)
        {
            printf("%02d      ", i);
        }

        printf("%+05d ", memory[i]);

        if (i % 10 == 9)
        {
            printf("\n");
        }
    }

    printf("============================================\n");
}