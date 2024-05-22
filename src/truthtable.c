#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAX_SIZE 100
 
typedef enum { AND, OR, NAND, NOR, XOR, NOT, PASS, DECODER, MULTIPLEXER } kind_t;


struct gate
{
    kind_t kind;
    int size; // indicates size of DECODER and MULTIPLEXER
    int paramCount;
    int *params; // length determined by kind and size;
    // includes inputs and outputs, indicated by variable numbers
};
typedef struct gate Gate;

struct dataStorage {
    Gate* gates[MAX_SIZE];
    int gateInputs;
    int gateOutputs;
    char* params[MAX_SIZE];
    int paramCount;
    int gateValues[MAX_SIZE];
    int Count;  
    
};
typedef struct dataStorage DigitC;

void performLogicalAND(int* storage, int inputdata1, int inputdata2, int outputdata) {
    storage[outputdata] = storage[inputdata1] && storage[inputdata2];
}

void performLogicalNAND(int* storage, int inputdata1, int inputdata2, int outputdata) {
    storage[outputdata] = !(storage[inputdata1] && storage[inputdata2]);
}

void performLogicalOR(int* storage, int inputdata1, int inputdata2, int outputdata) {
    storage[outputdata] = storage[inputdata1] || storage[inputdata2];
}

void performLogicalNOR(int* storage, int inputdata1, int inputdata2, int outputdata) {
    storage[outputdata] = !(storage[inputdata1] || storage[inputdata2]);
}

void performLogicalXOR(int* storage, int inputdata1, int inputdata2, int outputdata) {
    storage[outputdata] = storage[inputdata1] ^ storage[inputdata2];
}

void performLogicalNOT(int* storage, int inputdata, int outputdata) {
    storage[outputdata] = !storage[inputdata];
}

void performPass(int* storage, int inputdata, int outputdata) {
    storage[outputdata] = storage[inputdata];
}

void performDecode(int* storage, int size, int* inputdata, int* outputdata) {
    int sum = 0;

    int powerOfTwo = 1 << size;

    for (int i = 0; i < powerOfTwo; i++) {
        storage[outputdata[i]] = 0;
    }

    for (int i = 0; i < size; i++) {
        sum += storage[inputdata[i]] * (1 << (size - i - 1));
    }

    storage[outputdata[sum]] = 1;
}

void performMultiplex(int* storage, int size, int* inputdata, int* selectorIndex, int outputdata) {
    int sum = 0;
    int i = 0;

    while (i < size) {
        sum += storage[selectorIndex[i]] * (1 << (size - i - 1));
        i++;
    }

    storage[inputdata[outputdata]] = storage[inputdata[sum]];
}

Gate* initializeGate(kind_t kind, int paramCount) {
    Gate* storage = (Gate*)malloc(sizeof(Gate));
    storage->size = 0;
    
    storage->kind = kind;
    
    storage->paramCount = paramCount;
    storage->params = (int*)malloc(sizeof(int) * paramCount);
    return storage;
}

Gate* AndGate() {
    return initializeGate(AND, 3);
}

Gate* OrGate() {
    return initializeGate(OR, 3);
}

Gate* XorGate() {
    return initializeGate(XOR, 3);
}

Gate* NorGate() {
    return initializeGate(NOR, 3);
}

Gate* NandGate() {
    return initializeGate(NAND, 3);
}

Gate* NotGate() {
    return initializeGate(NOT, 2);
}

Gate* PassGate() {
    return initializeGate(PASS, 2);
}

Gate* DecoderGate(int size) {
    Gate* storage = (Gate*)malloc(sizeof(Gate));
    storage->kind = DECODER;
    storage->size = size;

    int powerOfTwo = 1 << size;

    storage->paramCount = size + powerOfTwo;
    storage->params = (int*)malloc(sizeof(int) * storage->paramCount);
    return storage;
}

Gate* MultiplexerGate(int size) {
    Gate* storage = (Gate*)malloc(sizeof(Gate));
    storage->size = size;
    storage->kind = MULTIPLEXER;
    

    int powerOfTwo = 1 << size;

    storage->paramCount = powerOfTwo + size + 1;
    storage->params = (int*)malloc(sizeof(int) * storage->paramCount);
    return storage;
}

 
 

 
void resetParam(DigitC* circuit) {
int i = 0;
while (i < circuit->gateInputs + circuit->gateOutputs) {
    circuit->gateValues[i] = 0;
    i++;
}
}



void addGate(DigitC* circuit, Gate* gate) {
    circuit->gates[circuit->Count] = gate;
    circuit->Count++;
}
int addOrFindParam(DigitC* circuit, char* paramName) {
    // Check if the parameter already exists
int i = 0;
while (i < circuit->paramCount) {
    if (strcmp(circuit->params[i], paramName) == 0) {
        return i; // Parameter already exists, return its index
    }
    i++;
}

    // If the parameter doesn't exist, add it
    int paramData = circuit->paramCount;

    circuit->params[paramData] = (char*)malloc(strlen(paramName) + 1);

    strcpy(circuit->params[paramData], paramName);
    
    circuit->paramCount++;
    return paramData;
}
void freeSpace(Gate* gate){
    free(gate->params);
    free(gate);
}

void freeGate(DigitC* circuit) {
    
    int i = 0;
    while (i < circuit->paramCount) {
        free(circuit->params[i]);
        i++;
    }

    i = 0;
    while (i < circuit->Count) {
        freeSpace(circuit->gates[i]);
        i++;
    }
}

void Stimulate(DigitC* circuit, int* inputs) {
    // Set input values using a while loop
    int i = 0;
    while (i < circuit->gateInputs) {
        circuit->gateValues[i] = inputs[i];
        i++;
    }
    // Process gates
    for (int i = 0; i < circuit->Count; i++) {
        Gate *gate = circuit->gates[i];
        switch (gate->kind) {
            case AND:
                performLogicalAND(circuit->gateValues, gate->params[0], gate->params[1], gate->params[2]);
                break;
            case OR:
                performLogicalOR(circuit->gateValues, gate->params[0], gate->params[1], gate->params[2]);
                break;
            case NOT:
                performLogicalNOT(circuit->gateValues, gate->params[0], gate->params[1]);
                break;
            case NAND:
                performLogicalNAND(circuit->gateValues, gate->params[0], gate->params[1], gate->params[2]);
                break;
            case NOR:
                performLogicalNOR(circuit->gateValues, gate->params[0], gate->params[1], gate->params[2]);
                break;
            case XOR:
                performLogicalXOR(circuit->gateValues, gate->params[0], gate->params[1], gate->params[2]);
                break;
            case PASS:
                performPass(circuit->gateValues, gate->params[0], gate->params[1]);
                break;
            case DECODER:
                performDecode(circuit->gateValues, gate->size, gate->params, gate->params + gate->size);
                break;
            case MULTIPLEXER:
                int inputSize = 1 << gate->size;
                performMultiplex(circuit->gateValues, gate->size, gate->params, gate->params + inputSize, gate->paramCount - 1);
                break;
            default:
                fprintf(stderr, "Error: Unexpected gate kind %d\n", gate->kind);
                break;
        }
    }
}




void initializeGatesFromFile(DigitC* circuit, FILE* f) {
    char type[MAX_SIZE];
    int number = 0;
    char name[MAX_SIZE];

    while ((number = fscanf(f, "%16s", type)) > 0) {
        Gate* gate = NULL;
        kind_t gateType;

        // Determine gate type
        switch (type[0]) {
            case 'A':
                gateType = AND;
                break;
            case 'O':
                gateType = OR;
                break;
            case 'X':
                gateType = XOR;
                break;
            case 'N':
                if (type[1] == 'O') {
                    gateType = NOR;
                } else {
                    gateType = NAND;
                }
                break;
            case 'M':
                gateType = MULTIPLEXER;
                break;
            case 'D':
                gateType = DECODER;
                break;
            case 'P':
                gateType = PASS;
                break;
            default:
                fprintf(stderr, "Unknown gate type: %s\n", type);
                continue;
        }

        // Create gate based on type
        switch (gateType) {
            case AND:
                gate = AndGate();
                break;
            case OR:
                gate = OrGate();
                break;
            case XOR:
                gate = XorGate();
                break;
            case NOR:
                gate = NorGate();
                break;
            case NAND:
                gate = NandGate();
                break;
            case MULTIPLEXER: {
                int input_size = 0;
                fscanf(f, "%d", &input_size);
                gate = MultiplexerGate(input_size);
                break;
            }
            case DECODER: {
                int input_size = 0;
                fscanf(f, "%d", &input_size);
                gate = DecoderGate(input_size);
                break;
            }
            case PASS:
                gate = PassGate();
                break;
            default:
                fprintf(stderr, "Unknown gate type: %d\n", gateType);
                continue;
        }

        // Add parameters to the gate
        if (gate != NULL) {
    for (int i = 0; i < gate->paramCount; i++) {
        fscanf(f, "%16s", name);
        
        int holder = addOrFindParam(circuit, name);
        
        if (holder < 0) {
                printf("Error: Unable to add or find parameter '%s'\n", name);

            
        }
        gate->params[i] = holder;

        // Use atoi to convert the string to an integer
        int value = atoi(name);
        circuit->gateValues[holder] = value;
    }

            }
            addGate(circuit, gate);
        }
    }


void printTruthTable(DigitC *dataBlock) {
    int numInputs = dataBlock->gateInputs;
    int numOutputs = dataBlock->gateOutputs;

    int powerOfTwo = 1 << numInputs;

    int *inputdatas = (int *)malloc(sizeof(int) * numInputs);

    for (int i = 0; i < powerOfTwo; i++) {
        // Generate binary representation of the current row
        for (int j = 0; j < numInputs; j++) {
            inputdatas[numInputs - j - 1] = (i >> j) & 1;
        }

        Stimulate(dataBlock, inputdatas);

        // Print input values
        int idx = 0;
        while (idx < numInputs) {
        printf("%d ", inputdatas[idx]);
        idx++;
        }

        printf("| ");

        // Print output values
       int idz = numInputs;
        while (idz < numInputs + numOutputs) {
        printf("%d", dataBlock->gateValues[idz]);
        if (idz != numInputs + numOutputs - 1) {
        printf(" ");
        }
        idz++;
        }


        printf("\n");

        resetParam(dataBlock);
    }

    free(inputdatas);
}
 
 
int main(int argc, char **argv) {
    DigitC digitalboard;
    char gateString[MAX_SIZE];
    int Inputs;
    digitalboard.Count = 0;
    int Outputs;
    
    digitalboard.paramCount = 0;
    char paramSymbol[MAX_SIZE];
    FILE *f = fopen(argv[1], "r");





    if (!f) {
        printf("Error opening file %s\n", argv[1]);
        return 0;
    }

   


    if (fscanf(f, "%16s", gateString) && fscanf(f, "%d", &Inputs)!= 1) {
        printf("Error reading gate type\n");
        fclose(f);
        return 1;
    }
    else{
        digitalboard.gateInputs = Inputs;
    }
    

    int loop = 0;
    while (loop < Inputs && fscanf(f, "%16s", paramSymbol) == 1) {
    addOrFindParam(&digitalboard, paramSymbol);
    loop++;
    }
    if (fscanf(f, "%16s", gateString) && fscanf(f, "%d", &Outputs)!= 1) {
        printf("Error reading gate type\n");
        fclose(f);
        return 1;
    }
    else{
        digitalboard.gateOutputs = Outputs;
    }
   
   
    int i = 0;
    while (i < Outputs && fscanf(f, "%16s", paramSymbol) == 1) {
    addOrFindParam(&digitalboard, paramSymbol);
    i++;
    }

    initializeGatesFromFile(&digitalboard, f);

    fclose(f);
    printTruthTable(&digitalboard);

    
    freeGate(&digitalboard);

    return 0;
}
