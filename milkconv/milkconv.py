#!/usr/bin/python3
import sys
import math
import png

def convert_to_header(model):

    mesh = model.meshes[0]

    base_path = sys.argv[1][0:sys.argv[1].rindex(".")]
    base_name = base_path[base_path.rindex("/") + 1:]
    header_name = base_path + ".h"
    type_base = base_name.replace(".", "_")
 
    fixed_mpath = model.materials[0].replace("Z:\\", "/").replace("\\", "/")

    pf = open(fixed_mpath, "rb")
    reader = png.Reader(file=pf)
    (img_w, img_h, img_iter, img_info) = reader.read()

    f = open(header_name, "w")

    f.write("#ifndef {}_H\n".format(type_base.upper()))
    f.write("#define {}_H\n".format(type_base.upper()))
    f.write("\n")
    f.write("#include \"triangle.h\"\n")
    f.write("\n")

    f.write("Triangle {}_triangles[] = {{\n".format(type_base))

    i = 0

    for tri in mesh.triangles:
        f.write("    {\n")
        f.write("        {\n")
        f.write("            {{ {}, {}, {}, 0xFFFFFF, {}, {} }},\n".format(
                mesh.vertices[tri.vertices[0]].x, mesh.vertices[tri.vertices[0]].y, mesh.vertices[tri.vertices[0]].z,
                int((mesh.vertices[tri.vertices[0]].v * img_h) + 0.5),
                int(img_w - (mesh.vertices[tri.vertices[0]].u * img_w) + 0.5)
        ))
        f.write("            {{ {}, {}, {}, 0xFFFFFF, {}, {} }},\n".format(
                mesh.vertices[tri.vertices[1]].x, mesh.vertices[tri.vertices[1]].y, mesh.vertices[tri.vertices[1]].z,
                int((mesh.vertices[tri.vertices[1]].v * img_h) + 0.5),
                int(img_w - (mesh.vertices[tri.vertices[1]].u * img_w) + 0.5)
        ))
        f.write("            {{ {}, {}, {}, 0xFFFFFF, {}, {} }}\n".format(
                mesh.vertices[tri.vertices[2]].x, mesh.vertices[tri.vertices[2]].y, mesh.vertices[tri.vertices[2]].z,
                int((mesh.vertices[tri.vertices[2]].v * img_h) + 0.5),
                int(img_w - (mesh.vertices[tri.vertices[2]].u * img_w) + 0.5)
        ))
        f.write("        },\n")
        f.write("        0xFFFFFF,\n")
        f.write("        { 0, 0, 0, 0, 0 }\n")
        if(i == (len(mesh.triangles) - 1)):
            f.write("    }\n")
        else:
            f.write("    },\n")

        i = i + 1

    f.write("};\n")
    psx_palette = []
    for rgb_triple in img_info["palette"]:
        (r, g, b) = rgb_triple
        out_pix = 0x8000 | (
            (((b & 0xF8) >> 4) << 10) |
            (((g & 0xF8) >> 4) <<  5) |
            (((r & 0xF8) >> 4) <<  0)
        )
        psx_palette.append(out_pix)

    f.write("uint16_t {}_pixdata[] = {{\n".format(type_base))

    i = 0

    for pix_row in img_iter:
        for pix in pix_row:
            if i != 0:
                f.write(", ")
            i = i + 1
            if i % 16 == 0:
                f.write("\n")
            f.write("{0:#04X}".format(psx_palette[pix]))

    f.write("\n};\n")

    pf.close()

    f.write("TriangleList {}_mesh = {{\n".format(type_base))
    f.write("    {},\n".format(len(mesh.triangles)))
    f.write("    &{}_triangles[0],\n".format(type_base))
    f.write("    {},\n".format(img_w))
    f.write("    {},\n".format(img_h))
    f.write("    &{}_pixdata[0]\n".format(type_base))
    f.write("};\n\n")

    f.write("\n")
    f.write("#endif //{}_H\n".format(type_base.upper()))

    f.close()

def normalize(model):

    mesh = model.meshes[0]
    tri = mesh.triangles[0]
    vtx = [mesh.vertices[tri.vertices[0]], mesh.vertices[tri.vertices[1]], mesh.vertices[tri.vertices[2]]]

    print("Vertices:\n\t({}, {}, {})\n\t({}, {}, {})\n\t({}, {}, {})".format(
        vtx[0].x, vtx[0].y, vtx[0].z,
        vtx[1].x, vtx[1].y, vtx[1].z,
        vtx[2].x, vtx[2].y, vtx[2].z
    ))

    vec_a = Vec3().Add(vtx[0]).Subtract(vtx[2])
    
    print("vec_a: ({}, {}, {})".format(vec_a.x, vec_a.y, vec_a.z))

    vec_b = Vec3().Add(vtx[1]).Subtract(vtx[2])

    print("vec_b: ({}, {}, {})".format(vec_b.x, vec_b.y, vec_b.z))

    nrm = vec_a.Cross(vec_b) 

    print("Unscaled normal: ({}, {}, {})".format(nrm.x, nrm.y, nrm.z))

    print("Normal magnitude: {}".format(nrm.Mag()))

    nrm = nrm.Scale(1/nrm.Mag())

    print("First normal: ({}, {}, {})".format(nrm.x, nrm.y, nrm.z))

class Vertex:
    flags = 0
    x = 0.0
    y = 0.0
    z = 0.0
    u = 0
    v = 0
    boneIndex = 0

class Vec3:
    x = 0.0
    y = 0.0
    z = 0.0

    def Subtract(self, arg):
        result = Vec3()
        result.x = self.x - arg.x
        result.y = self.y - arg.y
        result.z = self.z - arg.z
        return result

    def Add(self, arg):
        result = Vec3()
        result.x = self.x + arg.x
        result.y = self.y + arg.y
        result.z = self.z + arg.z
        return result

    def Cross(self, arg):
        result = Vec3()
        result.x = self.y*arg.z - self.z*arg.y
        result.y = self.z*arg.x - self.x*arg.z
        result.z = self.x*arg.y - self.y*arg.x
        return result

    def Mag(self):
        return math.sqrt(self.x*self.x + self.y*self.y + self.z*self.z)

    def Scale(self, factor):
        result = Vec3()
        result.x = self.x * factor
        result.y = self.y * factor
        result.z = self.z * factor
        return result

class Triangle:
    flags = 0
    vertices = []
    normals = []
    smoothingGroup = 0

    def __init__(self, flags, vtx0, vtx1, vtx2, nrm0, nrm1, nrm2, sm_grp):
        self.flags = flags
        self.vertices = [vtx0, vtx1, vtx2]
        self.normals = [nrm0, nrm1, nrm2]
        self.smoothingGroup = sm_grp

class Mesh:
    flags = 0
    name = ""
    vertices = []
    normals = []
    triangles = []
    material = 0

class Model:

    meshes = []
    materials = []

    def isBlank(self, l):
        return l.strip() == "" or l.strip().startswith("//")

    def __init__(self, file_path):

        f = open(file_path, "r")

        l = ""
        while not l.startswith("Meshes: "):
            l = f.readline()

        mesh_count = int(l.split(":")[1].strip())

        print("Importing {} mesh(es)".format(mesh_count))

        for i in range(0, mesh_count):

            cur_mesh = Mesh()
            self.meshes.append(cur_mesh)

            l = ""
            while self.isBlank(l):
                l = f.readline()

            s = l.split(" ")
            name = s[0].strip()[1:-1]
            flags = int(s[1].strip())
            material_index = int(s[2].strip())

            print("    Processing mesh #{}, named '{}'".format(i + 1, name))
            print("    Flags: {}".format(flags))
            print("    Material Idx: {}".format(material_index))

            cur_mesh.flags = flags
            cur_mesh.name = name
            cur_mesh.material = material_index

            l = ""
            while self.isBlank(l):
                l = f.readline()

            vertex_count = int(l.strip())

            print("    Importing {} vertices".format(vertex_count))

            for j in range(0, vertex_count):

                cur_vtx = Vertex()
                cur_mesh.vertices.append(cur_vtx)

                l = ""
                while self.isBlank(l):
                    l = f.readline()

                s = l.split(" ")
                vtx_flags = int(s[0].strip())
                vtx_x = float(s[1].strip())
                vtx_y = float(s[2].strip())
                vtx_z = float(s[3].strip())
                vtx_u = float(s[4].strip())
                vtx_v = float(s[5].strip())
                vtx_bone_index = int(s[6].strip())

                print("        Vertex #{}".format(j + 1))
                print("            Flags: {}".format(vtx_flags))
                print("            Coord: ({}, {}, {})".format(vtx_x, vtx_y, vtx_z))
                print("            UV: ({}, {})".format(vtx_u, vtx_v))
                print("            Bone Idx: {}".format(vtx_bone_index))

                cur_vtx.flags = vtx_flags
                cur_vtx.x = vtx_x
                cur_vtx.y = vtx_y
                cur_vtx.z = vtx_z
                cur_vtx.u = vtx_u
                cur_vtx.v = vtx_v
                cur_vtx.bone_index = vtx_bone_index

            l = ""
            while self.isBlank(l):
                l = f.readline()

            normal_count = int(l.strip())

            print("    Importing {} normals".format(normal_count))

            for j in range(0, normal_count):

                cur_nrm = Vec3()
                cur_mesh.normals.append(cur_nrm)

                l = ""
                while self.isBlank(l):
                    l = f.readline()

                s = l.split(" ")
                normal_x = float(s[0].strip())
                normal_y = float(s[1].strip())
                normal_z = float(s[2].strip())

                print("        Normal #{}: ({}, {}, {})".format(j + 1, normal_x, normal_y, normal_z))

                cur_nrm.x = normal_x
                cur_nrm.y = normal_y
                cur_nrm.z = normal_z

            l = ""
            while self.isBlank(l):
                l = f.readline()

            tri_count = int(l.strip())

            print("    Importing {} triangles".format(tri_count))

            for j in range(0, tri_count):

                l = ""
                while self.isBlank(l):
                    l = f.readline()

                s = l.split(" ")
                tri_flags = int(s[0].strip())
                tri_vtx1 = int(s[1].strip())
                tri_vtx2 = int(s[2].strip())
                tri_vtx3 = int(s[3].strip())
                tri_nrml1 = int(s[4].strip())
                tri_nrml2 = int(s[5].strip())
                tri_nrml3 = int(s[6].strip())
                tri_smoothing_grp = int(s[7].strip())

                print("    Triangle #{}:".format(j + 1))
                print("        Flags: {}".format(tri_flags))
                print("        Vertices: {}, {}, {}".format(tri_vtx1, tri_vtx2, tri_vtx3))
                print("        Normals: {}, {}, {}".format(tri_nrml1, tri_nrml2, tri_nrml3))
                print("        Smoothing group: {}".format(tri_smoothing_grp))

                cur_tri = Triangle(
                        tri_flags,
                        tri_vtx1, tri_vtx2, tri_vtx3,
                        tri_nrml1, tri_nrml2, tri_nrml3,
                        tri_smoothing_grp
                )

                cur_mesh.triangles.append(cur_tri)

            l = ""
            while self.isBlank(l):
                l = f.readline()

            matl_count = int(l.split(":")[1].strip())

            print("    Importing {} materials".format(matl_count))

            for j in range(0, matl_count):

                #Name
                l = ""
                while self.isBlank(l):
                    l = f.readline()

                #Ambient
                l = ""
                while self.isBlank(l):
                    l = f.readline()

                #Diffuse
                l = ""
                while self.isBlank(l):
                    l = f.readline()

                #Specular
                l = ""
                while self.isBlank(l):
                    l = f.readline()

                #Emissive
                l = ""
                while self.isBlank(l):
                    l = f.readline()

                #Shininess
                l = ""
                while self.isBlank(l):
                    l = f.readline()

                #Transparency
                l = ""
                while self.isBlank(l):
                    l = f.readline()

                #Path
                l = ""
                while self.isBlank(l):
                    l = f.readline()

                matl_path = l.strip()[1:-1]

                self.materials.append(matl_path)

                print("    Imported material '{}'".format(matl_path))

        f.close()

if len(sys.argv) < 2 or len(sys.argv) > 3:
    print("usgae: milkconv.py <source.txt>")
    quit()

model_data = Model(sys.argv[1])

if len(sys.argv) == 3:

    if sys.argv[2] != "normalize":
        print("Unrecognized option: '{}'".format(sys.argv[2]))
        quit()

    normalize(model_data)
else:
    convert_to_header(model_data)

