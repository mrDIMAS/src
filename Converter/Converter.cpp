// this converter used to convert raw data from 3ds max exporter to engine format
// (C) mrDIMAS 2014 - 2015

#include "Vector.h"
#include "Writer.h"
#include "Reader.h"
#include "Mesh.h"


struct Keyframe {
	Vector3 pos;
	Vector4 rot;

	Keyframe(Vector3 _pos, Vector4 _rot) {
		pos = _pos;
		rot = _rot;
	}
};

class Object {
public:
	Object() {

	}

	~Object() {

	}

	string mName;
	string mProperties;
	int mAnimated;
	Vector3 mPosition;
	Vector4 mRotation;
	int mSkinned;
	vector<Keyframe> mKeyframes;
	vector<Material> mMaterials;
	vector<shared_ptr<Mesh>> mMeshes;
	vector<string> mBones;
	vector<Vector3> mVertexCoords;
	vector<Vector2> mTexCoords;
	vector<Vector2> mSecondTexCoords;
};

class Light {
public:
	Vector3 mColor;
	float mInner, mOuter;
	float mRadius;
	Vector3 mPosition;
	Vector4 mRotation;
	string mName;
	float mBrightness;
	int mType;
};

class Multimesh {
private:
	void AddNewSubMesh() {
		auto mesh = make_shared<Mesh>();
		mesh->mat = material;
		meshes.push_back(mesh);
	}
public:
	vector<shared_ptr<Mesh>> meshes;
	Material material;

	Multimesh(Material mat) {
		material = mat;
		AddNewSubMesh();
	}

	~Multimesh() {

	}

	void AddVertex(const Vertex & v, const Weight & w) {
		meshes.back()->AddVertex(v, w);

		if (meshes.back()->indices.size() >= 65535) {
			AddNewSubMesh();
		}
	}

	void AddVertex(const Vertex & v) {
		meshes.back()->AddVertex(v);

		if (meshes.back()->indices.size() >= 65535) {
			AddNewSubMesh();
		}
	}
};


int main(int argc, char * argv[]) {
	cout << "**********************************************************************" << endl;
	cout << "Ruthenium Scene Converter. (C) mrDIMAS 2014-2017. All rights reserved." << endl;
	cout << "**********************************************************************" << endl << endl;
#ifndef _DEBUG
	if (argc < 2) {
		cout << "You must pass at least filename to convert!" << endl;
		getch();
		return 0;
	}

	string filename = argv[1];
#else
	string filename = "C:/Mine/release/data/maps/mine.scene";
#endif
	cout << "Reading " << filename << endl;

	Reader reader;

	vector<shared_ptr<Object>> objects;
	vector<Light> lights;
	map< string, string > hierarchy;

	int framesCount;

	if (reader.ReadFile(filename)) {
		cout << "Reading header" << endl;

		framesCount = reader.GetInteger();
		int numObjects = reader.GetInteger();
		int numMeshes = reader.GetInteger();
		int numLights = reader.GetInteger();

		cout << "   Frames count: " << framesCount << endl;
		cout << "   Object count: " << numObjects << endl;
		cout << "   Mesh count: " << numMeshes << endl;
		cout << "   Light count: " << numLights << endl << endl;

		cout << "Conversion started..." << endl;

		for (int objNum = 0; objNum < numMeshes; objNum++) {
			auto object = make_shared<Object>();

			object->mProperties = reader.GetString();

			int vertexCount = reader.GetInteger();
			int textureCoordCount = reader.GetInteger();
			int faceCount = reader.GetInteger();
			int isSkinned = reader.GetInteger();
			int hasAnimation = reader.GetInteger();

			object->mName = reader.GetString();
			object->mAnimated = hasAnimation;

#ifdef _DEBUG
			cout << "Reading object " << object->name << endl;
			cout << "   Vertex count: " << vertexCount << endl;
			cout << "   Texture coords count: " << textureCoordCount << endl;
			cout << "   Face count: " << faceCount << endl;
			cout << "   Is skinned: " << isSkinned << endl;
			cout << "   Animated: " << hasAnimation << endl;
#endif

			if (hasAnimation) {
				// read keyframes
				for (int frameNum = 0; frameNum < framesCount; frameNum++) {
					Vector3 pos = reader.GetVector();
					Vector4 rot = reader.GetQuaternion();

					object->mKeyframes.push_back(Keyframe(pos, rot));
				}
			} else {
				object->mPosition = reader.GetVector();
				object->mRotation = reader.GetQuaternion();
			}

			vector<shared_ptr<Multimesh>> meshes;

			// create materials
			int numMaterials = reader.GetInteger();

			for (int matID = 0; matID < numMaterials; matID++) {
				Material mat;

				string diffuse = reader.GetString();
				float opacity = reader.GetFloat();

				mat.diffuse = diffuse;
				mat.opacity = opacity;

				if (diffuse.size()) {
					diffuse.insert(diffuse.find_last_of('.'), "_normal");

					mat.normal = diffuse;
				}

				object->mMaterials.push_back(mat);
				meshes.push_back(make_shared<Multimesh>(mat));
			}

			// read vertices
			object->mVertexCoords.reserve(vertexCount);
			for (int i = 0; i < vertexCount; ++i) {
				object->mVertexCoords.push_back(reader.GetVector());
			}

			vector< Weight > weights;
			weights.reserve(vertexCount);

			// read bones
			if (isSkinned) {
				// read weights for each vertex
				for (int i = 0; i < vertexCount; i++) {
					Weight weigth;

					weigth.boneCount = reader.GetInteger();

					for (int k = 0; k < weigth.boneCount; k++) {
						weigth.bones[k].id = reader.GetInteger();
						weigth.bones[k].weight = reader.GetFloat();
					}

					weights.push_back(weigth);
				}
			}

			int numMaps = reader.GetInteger();
			int numMapChannels = reader.GetInteger();

			if (numMapChannels == 1) {
				// read texture coords
				object->mTexCoords.reserve(textureCoordCount);
				for (int i = 0; i < textureCoordCount; ++i) {
					object->mTexCoords.push_back(reader.GetBareVector2());
				}
				// read texture coords
				object->mSecondTexCoords.reserve(textureCoordCount);
				for (int i = 0; i < textureCoordCount; ++i) {
					object->mSecondTexCoords.push_back(object->mTexCoords[i]);
				}
			}

			if (numMapChannels == 2) {
				// read texture coords
				object->mTexCoords.reserve(textureCoordCount);
				for (int i = 0; i < textureCoordCount; ++i) {
					object->mTexCoords.push_back(reader.GetBareVector2());
				}
				// read texture coords
				object->mSecondTexCoords.reserve(textureCoordCount);
				for (int i = 0; i < textureCoordCount; ++i) {
					object->mSecondTexCoords.push_back(reader.GetBareVector2());
				}
			}

			if (object->mTexCoords.size() == 0) {
				object->mTexCoords.push_back(Vector2(0, 0));
			}

			// read faces and construct meshes
			for (int i = 0; i < faceCount; ++i) {
				int fa = reader.GetInteger();
				int fb = reader.GetInteger();
				int fc = reader.GetInteger();

				int ta = reader.GetInteger();
				int tb = reader.GetInteger();
				int tc = reader.GetInteger();

				size_t faceMaterialID = reader.GetInteger();

				Vector3 aNormal = reader.GetVector();
				Vector3 bNormal = reader.GetVector();
				Vector3 cNormal = reader.GetVector();

				if (faceMaterialID >= numMaterials) {
					continue;
				}

				auto currentMultimesh = meshes[faceMaterialID];

				if (isSkinned) {
					if (textureCoordCount) {
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fa], &object->mTexCoords[ta], &object->mSecondTexCoords[ta], aNormal), weights[fa]);
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fb], &object->mTexCoords[tb], &object->mSecondTexCoords[tb], bNormal), weights[fb]);
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fc], &object->mTexCoords[tc], &object->mSecondTexCoords[tc], cNormal), weights[fc]);
					} else {
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fa], &object->mTexCoords[0], &object->mSecondTexCoords[ta], aNormal), weights[fa]);
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fb], &object->mTexCoords[0], &object->mSecondTexCoords[tb], bNormal), weights[fb]);
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fc], &object->mTexCoords[0], &object->mSecondTexCoords[tc], cNormal), weights[fc]);
					}
				} else {
					if (textureCoordCount) {
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fa], &object->mTexCoords[ta], &object->mSecondTexCoords[ta], aNormal));
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fb], &object->mTexCoords[tb], &object->mSecondTexCoords[tb], bNormal));
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fc], &object->mTexCoords[tc], &object->mSecondTexCoords[tc], cNormal));
					} else {
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fa], &object->mTexCoords[0], &object->mTexCoords[0], aNormal));
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fb], &object->mTexCoords[0], &object->mTexCoords[0], bNormal));
						currentMultimesh->AddVertex(Vertex(&object->mVertexCoords[fc], &object->mTexCoords[0], &object->mTexCoords[0], cNormal));
					}
				}

			}

			for (auto multimesh : meshes) {
				for (auto & mesh : multimesh->meshes) {
					object->mMeshes.push_back(std::move(mesh));
				}
			}

			for (auto mesh : object->mMeshes) {
				mesh->CalculateTangent();
				mesh->CalculateAABB();
			}

			object->mSkinned = isSkinned;

			objects.push_back(object);
		}

		// read lights
		for (int objNum = 0; objNum < numLights; objNum++) {
			Light light;

			light.mName = reader.GetString();
			light.mType = reader.GetInteger();
			light.mColor.x = reader.GetInteger();
			light.mColor.y = reader.GetInteger();
			light.mColor.z = reader.GetInteger();
			light.mRadius = reader.GetFloat();
			light.mBrightness = reader.GetFloat();
			light.mPosition = reader.GetVector();

			if (light.mType == 1) { // spot
				light.mInner = reader.GetFloat();
				light.mOuter = reader.GetFloat();
				light.mRotation = reader.GetQuaternion();
			}
			lights.push_back(light);
		}

		// read hierarchy
		for (int objNum = 0; objNum < numObjects; objNum++) {
			string objectName = reader.GetString();
			string parentName = reader.GetString();

			hierarchy[objectName] = parentName;
		}
	}

	// now write computed data
	Writer writer(filename);

	writer.WriteInteger(objects.size() + lights.size());
	writer.WriteInteger(objects.size());
	writer.WriteInteger(lights.size());
	writer.WriteInteger(framesCount);

	cout << endl << "Convertion done!" << endl << endl;
	cout << "Writing computed data..." << endl;
	cout << "   Total objects: " << objects.size() + lights.size() << endl;
	cout << "   Total mesh-objects: " << objects.size() << endl;
	cout << "   Total lights: " << lights.size() << endl;
	cout << "   Total frames: " << framesCount << endl;

	for (auto obj : objects) {
#ifdef _DEBUG
		cout << "Write object: " << obj->name << endl;
		cout << "   Mesh count: " << obj->meshes.size() << endl;
#endif

		writer.WriteVector(obj->mPosition);
		writer.WriteQuaternion(obj->mRotation);
		writer.WriteInteger(obj->mAnimated);
		writer.WriteInteger(obj->mSkinned);
		writer.WriteInteger(obj->mMeshes.size());
		writer.WriteInteger(obj->mKeyframes.size());

		writer.WriteString(obj->mProperties);
		writer.WriteString(obj->mName);

		for (int keyFrameNum = 0; keyFrameNum < obj->mKeyframes.size(); keyFrameNum++) {
			Keyframe & kf = obj->mKeyframes[keyFrameNum];
			writer.WriteVector(kf.pos);
			writer.WriteQuaternion(kf.rot);
		}

		for (int meshNum = 0; meshNum < obj->mMeshes.size(); meshNum++) {
			auto & mesh = obj->mMeshes[meshNum];

			writer.WriteInteger(mesh->vertices.size());
			writer.WriteInteger(mesh->indices.size());
			writer.WriteVector(mesh->min);
			writer.WriteVector(mesh->max);
			writer.WriteVector(mesh->center);
			writer.WriteFloat(mesh->radius);
			writer.WriteString(mesh->mat.diffuse);
			writer.WriteString(mesh->mat.normal);
			writer.WriteFloat(mesh->mat.opacity);

#ifdef _DEBUG
			cout << "   Write mesh: " << meshNum << endl;
			cout << "       Vertex count: " << mesh->vertices.size() << endl;
			cout << "       Index count: " << mesh->indices.size() << endl;
			cout << "       Face count: " << mesh->indices.size() / 3 << endl;
			cout << "       Diffuse: " << mesh->mat.diffuse << endl;
			cout << "       Normal: " << mesh->mat.normal << endl;
#endif

			for (int vertexNum = 0; vertexNum < mesh->vertices.size(); vertexNum++) {
				Vertex & v = mesh->vertices[vertexNum];

				writer.WriteVector(*v.pos);
				writer.WriteVector(v.nor);
				writer.WriteVector2(*v.tex);
				writer.WriteVector2(*v.secondTex);
				writer.WriteVector(v.tan);
			}

			for (int indexNum = 0; indexNum < mesh->indices.size(); indexNum++) {
				writer.WriteShort(mesh->indices[indexNum]);
			}

			if (obj->mSkinned) {
				for (int weightNum = 0; weightNum < mesh->weights.size(); weightNum++) {
					Weight & w = mesh->weights[weightNum];

					writer.WriteInteger(w.boneCount);

					for (int k = 0; k < w.boneCount; k++) {
						writer.WriteInteger(w.bones[k].id);
						writer.WriteFloat(w.bones[k].weight);
					}
				}
			}
		}
	}

	for (int i = 0; i < lights.size(); i++) {
		Light & light = lights[i];

		writer.WriteString(light.mName);
		writer.WriteInteger(light.mType);
		writer.WriteVector(light.mColor);
		writer.WriteFloat(light.mRadius);
		writer.WriteFloat(light.mBrightness);
		writer.WriteVector(light.mPosition);

		if (light.mType == 1) { // free spot
			writer.WriteFloat(light.mInner);
			writer.WriteFloat(light.mOuter);
			writer.WriteQuaternion(light.mRotation);
		}
	}

	for (auto it = hierarchy.begin(); it != hierarchy.end(); ++it) {
		writer.WriteString(it->first);
		writer.WriteString(it->second);
	}

	cout << "Complete!" << endl;

	return 0;
}