CXX:= g++
CXX_FLAGS:= -std=c++17

BIN:= bin
DOCS:= docs
SRC:= src
INCLUDE:= .
LIBRARIES:= -lSDL3 -lSDL3_image -lSDL3_ttf
EXECUTABLE:= VisualNovel.exe

all: $(BIN)/$(EXECUTABLE) copy-assets

copy-assets:
	@mkdir -p $(BIN)/images
	@mkdir -p $(BIN)/fonts
	@cp -r images/* $(BIN)/images/ 2>/dev/null || true
	@cp -r fonts/* $(BIN)/fonts/ 2>/dev/null || true
	@cp SDL/lib/x64/SDL3.dll $(BIN)/ 2>/dev/null || true
	@cp SDL_image/lib/x64/SDL3_image.dll $(BIN)/ 2>/dev/null || true
	@cp SDL_ttf/lib/x64/SDL3_ttf.dll $(BIN)/ 2>/dev/null || true

run: clean all
	@clear
	./$(BIN)/$(EXECUTABLE)

$(BIN):
	@mkdir -p $(BIN)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp | $(BIN)
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -ISDL/include -ISDL_image/include -ISDL_ttf/include $^ -o $@ -LSDL/lib/x64 -LSDL_image/lib/x64 -LSDL_ttf/lib/x64 $(LIBRARIES)

clean:
	rm -rf $(BIN)

docs: $(SRC)/*.cpp
	doxygen Doxyfile

clean-docs:
	rm -rf $(DOCS) html latex
