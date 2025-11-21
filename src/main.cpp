#include <raylib.h>
#include <iostream>
#include <random>
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


//============================ Generic Classes =========================//
class Pillar{
    public :
    float height;
    float width;
    Color color;
    Vector2 pos;
    void draw(){
        DrawRectangle(pos.x, pos.y, width, height, color);
    }
};

class Sorter{
    public :
    virtual void step() = 0;
    virtual void reset(int* array, int s) = 0;
    virtual int activeIndex() = 0;
    virtual bool isSorted() = 0;
    virtual const char* name() = 0;
};
//====================================================================//


//============================ Helpers =========================//
void fillArray(int* array, int size, int screenHeight){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, screenHeight/1.5);

    for(int i = 0; i < size; i++){
        array[i] = i + 1 * distrib(gen);
    }
}

Color HSVtoRGB(float h, float s, float v) {
    float r = 0, g = 0, b = 0;

    if (s <= 0.0f) {
        r = g = b = v;
    } else {
        h = fmodf(h, 1.0f);
        if (h < 0) h += 1.0f;
        float hf = h * 6.0f;
        int sector = (int)hf;
        float f = hf - sector;
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));

        switch (sector) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            case 5: default: r = v; g = p; b = q; break;
        }
    }

    Color out;
    out.r = (unsigned char)(Clamp(r, 0.0f, 1.0f) * 255.0f);
    out.g = (unsigned char)(Clamp(g, 0.0f, 1.0f) * 255.0f);
    out.b = (unsigned char)(Clamp(b, 0.0f, 1.0f) * 255.0f);
    out.a = 255;
    return out;
}
//==============================================================//


//============================ Sorting Algorithms =========================//
class BubbleSort : public Sorter{
    public :
    int* arr;
    int size;
    int i,j;

    BubbleSort() : arr(nullptr), size(0), i(0), j(0) {}

    void step() override {
        if(isSorted()) return;
        if(j < size - i - 1){
            if(arr[j] > arr[j + 1]){
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
            j++;
        } else {
            j = 0;
            i++;
        }
    }
    void reset(int* array, int s) override {
        arr = array;
        size = s;
        i = 0;
        j = 0;
    }
    int activeIndex() override {
        return j;
    }
    bool isSorted() override {
        return i >= size - 1;
    }
    const char* name() override {
        return "Bubble";
    }
};

class InsertionSort : public Sorter {
public:
    int* arr;
    int size;

    int i, j;
    int key;
    bool holdingKey;

    InsertionSort() : arr(nullptr), size(0), i(1), j(0), holdingKey(false) {}

    void reset(int* array, int s) override {
        arr = array;
        size = s;
        i = 1;
        j = 0;
        holdingKey = false;
    }

    void step() override {
        if (isSorted()) return;

        if (!holdingKey) {
            key = arr[i];
            j = i - 1;
            holdingKey = true;
        }

        if (j >= 0 && arr[j] > key) {
            arr[j+1] = arr[j];
            j--;
        } else {
            arr[j+1] = key;
            holdingKey = false;
            i++;
        }
    }

    int activeIndex() override { return j; }

    bool isSorted() override { return i >= size; }

    const char* name() override { return "Insertion"; }
};

class SelectionSort : public Sorter{
    public :
    int* arr;
    int size;
    int i, j, minIndex;

    SelectionSort() : arr(nullptr), size(0), i(0), j(1), minIndex(0) {}

    void step() override {
        if(isSorted()) return;
        if(i < size - 1){
            if(j < size){
                if(arr[j] < arr[minIndex]){
                    minIndex = j;
                }
                j++;
            } else {
                int temp = arr[i];
                arr[i] = arr[minIndex];
                arr[minIndex] = temp;
                i++;
                minIndex = i;
                j = i + 1;
            }
        }
    }
    void reset(int* array, int s) override {
        arr = array;
        size = s;
        i = 0;
        j = 1;
        minIndex = 0;
    }
    int activeIndex() override {
        return i;
    }
    bool isSorted() override {
        return i >= size - 1;
    }
    const char* name() override {
        return "Selection";
    }
};

class GnomeSort : public Sorter {
public:
    int* arr;
    int size;
    int index;

    GnomeSort() : arr(nullptr), size(0), index(1) {}

    void reset(int* array, int s) override {
        arr = array;
        size = s;
        index = 1;
    }

    void step() override {
        if (isSorted()) return;

        if (index == 0) {
            index = 1;
            return;
        }

        if (arr[index] >= arr[index - 1]) {
            index++;
        } else {
            int temp = arr[index];
            arr[index] = arr[index - 1];
            arr[index - 1] = temp;
            index--;
        }
    }

    int activeIndex() override { return index; }

    bool isSorted() override { return index >= size; }

    const char* name() override { return "Gnome"; }
};

class BogoSort : public Sorter {
public:
    int* arr;
    int size;

    BogoSort() : arr(nullptr), size(0) {}

    void reset(int* array, int s) override {
        arr = array;
        size = s;
    }

    void shuffle() {
        for (int i = size - 1; i > 0; i--) {
            int j = GetRandomValue(0, i);
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    void step() override {
        if (isSorted()) return;
        shuffle();
    }

    int activeIndex() override {
        return 0;
    }

    bool isSorted() override {
        for (int i = 0; i < size - 1; i++)
            if (arr[i] > arr[i + 1]) return false;
        return true;
    }

    const char* name() override {
        return "Bogo";
    }
};

class MiracleSort : public Sorter {
public:
    int* arr;
    int size;

    MiracleSort() : arr(nullptr), size(0) {}

    void reset(int* array, int s) override {
        arr = array;
        size = s;
    }

    void step() override {}

    int activeIndex() override {
        return 0;
    }

    bool isSorted() override {
        return true;
    }

    const char* name() override {
        return "Miracle";
    }
};
//=======================================================================//

//============================ Main Function =========================//
int main() 
{
    
    // Window Settings
    int screenWidth = 1920 / 2;
    int screenHeight = 1080 / 2;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    
    InitWindow(screenWidth, screenHeight, "Sorting Visualiser By Adam");
    SetWindowIcon(LoadImage("icon.png"));
    SetTargetFPS(60);
    
    // Logic
    int arraySize = 99;
    int values[arraySize];
    fillArray(values, arraySize, screenHeight);

    // Register Sorting Algorithms
    std::vector<Sorter*> algorithms;
    algorithms.push_back(new BubbleSort());
    algorithms.push_back(new InsertionSort());
    algorithms.push_back(new SelectionSort());
    algorithms.push_back(new GnomeSort());
    algorithms.push_back(new BogoSort());
    algorithms.push_back(new MiracleSort());

    float stepDelay = 0.1f;
    float stepTimer = 0.0f;

    bool paused = true;
    bool shouldReset = false;
    bool rgbMode = false;

    int selected = 0;
    const char* nameList = "Bubble Sort;Insertion Sort;Selection Sort;Gnome Sort;Bogo Sort;Miracle Sort";
    
    // Main Loop
    while (!WindowShouldClose())
    {
        // Update
        screenHeight = GetScreenHeight();
        screenWidth = GetScreenWidth();

        float dt = GetFrameTime();
        stepTimer += dt;

        //======================== Drop Down =========================//
        static bool editMode = false;

        if (GuiDropdownBox({220, 10, 200, 50}, nameList, &selected, editMode)) {
            editMode = !editMode;
        }
        //============================================================//
        
        Sorter* active = algorithms[selected];

        //======================== Pause =========================//
        if(GuiButton((Rectangle){430, 10, 200, 50}, paused ? "Play" : "Pause")){
            paused = !paused;
        }
        //=======================================================//

        //======================== Reset =========================//
        if(GuiButton((Rectangle){10, 10, 200, 50}, "Reset")){
            shouldReset = true;
        }

        if(shouldReset){
            fillArray(values, arraySize, screenHeight);
            active->reset(values, arraySize);
            stepTimer = 0.0f;
            shouldReset = false;
        }
        //=======================================================//

        //======================== RGB Mode =========================//
        if(GuiButton((Rectangle){10, 70, 200, 50}, "Toggle RGB Mode")) {
            rgbMode = !rgbMode;
        }
        //=======================================================//

        //======================== Speed =========================//
        GuiSlider((Rectangle){700, 10, 200, 50}, "Fast", "Slow", &stepDelay, 0.001f, 1.0f);
        //=======================================================//
        
        if(!paused){
            if(!active->isSorted() && stepTimer >= stepDelay){
                active->step();
                stepTimer = 0.0f;
            }
        }
        
        int activeIndex = active->activeIndex();

        // Drawwing
        BeginDrawing();
        ClearBackground(BLACK);

        for(int i = 0; i < arraySize; i++){
            Pillar p;
            p.width = (float)screenWidth / arraySize;
            p.height = (float)values[i];
            p.pos = {(float)i * p.width, (float)screenHeight - p.height };
            
            if(rgbMode) {
                float t = GetTime();
                float hue = fmodf((i / (float)arraySize) + t * 0.1f, 1.0f);
                i == activeIndex ? p.color = RED : p.color = HSVtoRGB(hue, 1.0f, 1.0f);
            }
            else {
                i == activeIndex ? p.color = RED : p.color = WHITE;
            }
            p.draw();
        }
        EndDrawing();
    }
    CloseWindow();
}
//===============================================================//