#include "../include/render.h"

int main (int argc, char* argv[]) {
  //Default properties
  std::map<std::string, std::string> properties = { {NAME, ""},
                                                    {TYPE, ""},
                                                    {CODIFICATION,""},
                                                    {SIZE_HEIGHT, ""},
                                                    {SIZE_WIDTH, ""},
                                                    {UPPER_LEFT, ""},
                                                    {UPPER_RIGHT, ""},
                                                    {LOWER_LEFT, ""},
                                                    {LOWER_RIGHT, ""}
                                                  };
  std::string format = "P6";

  //== READ FILE
  if (argc < 2 ){
    std::cerr << "Gibe me input, plx" << '\n';
  }
  std::string file_name = argv[1];
  std::ifstream file(file_name.c_str(), std::ifstream::in);
  std::string content = "";
  if (file.is_open()) {
    while(!file.eof()){
      getline(file, content);
      std::istringstream field(content);
      //TODO Check if has comments to delete before the parse
      int limiter = content.find_first_of("=");
      if (limiter != std::string::npos) {
        std::string key = content.substr(0,limiter);
        key = removeSpaces(key); //TODO:: also put all Capital letter or Small Letter to make a better comparsion (INSENSITIVE VERIFICATION)
        std::string val = content.substr(limiter+1,content.length());
        if ( properties.find(key) == properties.end() ) {
          //Nothing to do. Not a key defined.
        } else {
          //Update value
          //TODO maybe use a token to check and etc
          properties[key] = val;
        }
      }
    }
  } else{
    std::cerr << "Not a correct file: " << file_name << '\n';
  }

  //=== PARSE INFO
  properties[NAME] = removeSpaces(properties[NAME]);
  properties[CODIFICATION] = removeSpaces(properties[CODIFICATION]);

  int n_col(std::stoi(properties[SIZE_WIDTH]));
  int n_row(std::stoi(properties[SIZE_HEIGHT]));

  background bg;
  std::stringstream ul( properties[UPPER_LEFT] ),
                    ur( properties[UPPER_RIGHT] ),
                    ll( properties[LOWER_LEFT] ),
                    lr( properties[LOWER_RIGHT] );

  for (int i = 0; i < NB_CHANNEL; i++){

    ul >> bg.upper_left.e[i];
    ur >> bg.upper_right.e[i];
    ll >> bg.lower_left.e[i];
    lr >> bg.lower_right.e[i];
  }

  //TODO: Treat TYPE/CODIFICATION
  bool is_binary = false;
  if (properties[CODIFICATION] == "binary"){
    is_binary = true;
  }
  else if (properties[CODIFICATION] == "ascii") {
    is_binary = false;
  } else{
    std::cerr << "Codification not accepted (yet)" << std::endl;
  }

  //Image image = new Image(properties[NAME], n_col, n_row);
  int size = n_col*n_row;
  Camera* c = new Camera();

  std::vector<Object*> objects;
  objects.push_back(new Sphere((Point3(0,-100.5,-3)), 99.f));
  objects.push_back(new Sphere((Point3(0.3,0,-1)), 0.4));
  objects.push_back(new Sphere((Point3(0,1,-2)), 0.6));
  objects.push_back(new Sphere((Point3(-0.4,0,-3)), 0.7));

  if (is_binary){
    //image.create_by_binary();
    format = "P6";
    std::ofstream image (properties[NAME], std::ios::out | std::ios::trunc | std::ios::binary);

    char * buffer = (char*) malloc(size*3*sizeof(char));
    int add = 0;

    for(int i = n_row-1; i >=0; i--){
      for(int j = 0; j < n_col; j++){
        float u = (float)j/(float)n_col;
        float v = (float)i/(float)n_row;
        Ray r(c->origin(), c->llc()+(u*c->horizontal())+(v*c->vertical()));
        Color3 bi_int = find_color(bg, std::make_pair(i,j), std::make_pair(n_col,n_row), r, objects);
        buffer[add++] = char(bi_int.e[0]);
        buffer[add++] = char(bi_int.e[1]);
        buffer[add++] = char(bi_int.e[2]);
      }
    }

    if (image){
      //Header
      image << format << "\n" << n_col << " " << n_row << "\n" << MAX << "\n";
      //Content
      image.write((char *)buffer, size*3);
      delete[] buffer;
      image.close();
    } else {
      std::cout << "vixe, problem with " << properties[NAME] << std::endl;
    }

  }

  else { //is ascii
    //image.create_by_ascii();
    format = "P3";
    std::ofstream image (properties[NAME], std::fstream::out | std::ios::trunc );

    if(!image){
      std::cout << "vixe, problem with " << properties[NAME] << std::endl;
    }
    image << format << "\n" << n_col << " " << n_row << "\n" << MAX << "\n";
    for(int i = n_row-1; i >=0; i--){
      for(int j = 0; j < n_col; j++){
        float u = (float)j/(float)n_col;
        float v = (float)i/(float)n_row;
        Ray r(c->origin(), c->llc()+(u*c->horizontal())+(v*c->vertical()));
        Color3 bi_int = find_color(bg, std::make_pair(i,j), std::make_pair(n_col,n_row), r, objects);
        image << int(bi_int.e[0]) << " " << int(bi_int.e[1]) << " " << int(bi_int.e[2]) << " ";
      }
      image << "\n";
    }
    image.close();
  }
}
