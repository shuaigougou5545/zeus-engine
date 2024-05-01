#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;

    Vertex() = default;
    Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty) : pos(glm::vec3(px, py, pz)), normal(glm::vec3(nx, ny, nz)), texCoord(glm::vec2(tx, ty)) {}

    bool operator==(const Vertex& other) const {
        return pos == other.pos &&
               normal == other.normal &&
               texCoord == other.texCoord;
    }
};

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

struct AABB {
    glm::vec3 min; 
    glm::vec3 max;
    glm::vec3 center;
    glm::vec3 extend; // 半长轴
};

class Model {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::string model_name;
    AABB aabb;

    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    void normalize(); // move to center & normalize position
};

class ModelOBJ : public Model {
// OBJ
public:
    ModelOBJ() = default;
    ModelOBJ(std::string file_name) { load(file_name); }

    void load(std::string file_name);
};

class ModelBox : public Model {
public:
    ModelBox() { load(1.0f, 1.0f, 1.0f); model_name = "box"; }
    ModelBox(const std::string& name) { load(1.0f, 1.0f, 1.0f); model_name = name; }
    
    void load(float width, float height, float depth);
};

class ModelQuad : public Model {
public:
    ModelQuad() { load(); model_name = "quad"; }
    ModelQuad(const std::string& name) { load(); model_name = name; }

    void load();
};

class ModelLibrary {
public:
    void add(const std::string& name, Model& model) {
        model_dict.emplace(name, std::make_shared<Model>(model));
        model_names.push_back(name);
    }

    std::shared_ptr<Model> get(const std::string& name) {
        auto it = model_dict.find(name);
        if (it != model_dict.end()) {
            return it->second;
        } else {
            throw std::runtime_error("Model not found: " + name);
        }
    }

    void loadModels(const std::string& file_path = "../resources/obj/");
    
    std::unordered_map<std::string, std::shared_ptr<Model>> model_dict;
    std::vector<std::string> model_names;
};

class ModelManager {
public:
    ModelManager() {}
    ModelManager(const Model& m) { model = std::make_shared<Model>(m); init(); }
    ~ModelManager();
    void init();
    void draw();

    void reloadModel(const Model& m);

    unsigned int VBO, EBO, VAO;
    std::shared_ptr<Model> model = nullptr;

    // material
    glm::vec3 albedo = glm::vec3(1.0f);
    float metallic = 0.7f;
    float roughness = 0.3f;
};

class ModelManagerList {
public:
    void add(std::shared_ptr<ModelManager> model_manager) {
        object_list.push_back(model_manager);
    }

    int count()
    {
        return object_list.size();
    }

    std::shared_ptr<ModelManager> get(int index)
    {
        return object_list[index];
    }

    std::vector<std::shared_ptr<ModelManager>> object_list;
};


#endif // MODEL_H