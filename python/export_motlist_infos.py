import json
import os
from termcolor import colored, cprint

from BinaryStream import *


CURRENT_DIR_PATH = os.path.dirname(__file__)
DMC_DIR = os.path.abspath(os.path.join(CURRENT_DIR_PATH, "../../DMC"))

motlist_file_path = os.path.join(DMC_DIR, "re_chunk_000.pak.patch_005/natives/x64/animation/player/pl0800/motlist/pl0800_yamato.motlist.85")


def ReadUByte(bs: BinaryStream, readAt):
    pos = bs.tell()
    bs.seek(readAt)
    value = bs.read_uint8()
    bs.seek(pos)
    return value

def ReadUInt64(bs: BinaryStream, readAt):
    pos = bs.tell()
    bs.seek(readAt)
    value = bs.read_uint64()
    bs.seek(pos)
    return value

def ReadUInt(bs: BinaryStream, readAt):
    pos = bs.tell()
    bs.seek(readAt)
    value = bs.read_uint32()
    bs.seek(pos)
    return value


def ReadUShort(bs: BinaryStream, tell):
    pos = bs.tell()
    bs.seek(tell)
    out = bs.read_uint16()
    bs.seek(pos)
    return out

def ReadString(bs: BinaryStream):
    resultString = ""
    c = bs.read_uint8()
    while c > 0:
        resultString += chr(c)
        c = bs.read_uint8()
    return resultString

def ReadUnicodeString(bs: BinaryStream):
    numZeroes = 0
    resultString = ""
    while numZeroes < 2:
        c = bs.read_uint8()
        if c == 0:
            numZeroes += 1
            continue
        else:
            numZeroes = 0
        resultString += chr(c)
    return resultString


def readUnicodeStringAt(bs: BinaryStream, tell):
    string = []
    pos = bs.tell()
    bs.seek(tell)
    while ReadUShort(bs, bs.tell()) != 0:
        string.append(bs.read_uint8())
        bs.seek(1, 1)
    bs.seek(pos)
    buff = struct.pack("<" + "b" * len(string), *string)
    return str(buff, "utf-8")

def skipToNextLine(bs):
	bs.seek(bs.tell() + 16 - (bs.tell() % 16))

class MotFile:
    
    def __init__(self, dataBytesArray: bytes, motlist=[], start=0):
        self.bs = BinaryStream(dataBytesArray)
        bs = self.bs
        self.start = start
        self.anim = None
        self.frameCount = 0
        self.motlist = motlist
        self.HEADER_version = motlist.version
        self.bones = []
        self.version = bs.read_uint32()
        bs.seek(12)
        self.motSize = bs.read_uint32()
        self.offsToBoneHdrOffs = bs.readUInt64()
        self.boneHdrOffset = 0
        self.boneClipHdrOffset = bs.readUInt64()
        bs.skip(8)
        if self.version >= 456:
            bs.skip(8)
            self.clipFileOffset = bs.readUInt64()
            self.jmapOffset = bs.readUInt64()
            self.exDataOffset  = bs.readUInt64()
            bs.skip(16)
        else:
            self.jmapOffset = bs.readUInt64()
            self.clipFileOffset = bs.readUInt64()
            bs.skip(16)
            self.exDataOffset = bs.readUInt64()
        nameOffs = bs.readUInt64()
        self.name = readUnicodeStringAt(bs, nameOffs)
        self.frameCount = bs.readFloat()
        # self.name += " (" + str(int(self.frameCount)) + " frames)"
        self.blending = bs.readFloat()
        self.uknFloat0 = bs.readFloat()
        self.uknFloat0 = bs.readFloat()
        self.boneCount = bs.readShort()
        self.boneClipCount = bs.readShort()
        self.clipCount = bs.readByte()
        self.uknCount = bs.readByte()
        self.frameRate = bs.readShort()
        self.uknCount2 = bs.readShort()
        self.ukn3 = bs.readShort()
        self.boneHeaders = []
        self.boneClipHeaders = []
        self.kfBones = []
        self.doSkip = False

    def export(self):
        bs = self.bs
        bs.seek(self.clipFileOffset)
        clipOffset = []
        print("clipCount: %s"%(self.clipCount))
        for i in range(self.clipCount):
            clipOffset.append(bs.read_uint64())
        if clipOffset[0] > 0:
            bs.seek(clipOffset[0])
        else:
            bs.skip(16)
        clips = []
        for clipIdx in range(self.clipCount):
            print("clipIdx: %s"%(clipIdx))
            if ReadUInt(bs, bs.tell()) !=1346980931 :
                A1 = bs.read_uint64()
                clipHdrOffs = bs.read_uint64()
                Offs = bs.read_uint64()
                D2 = bs.read_uint32()
                numFloats = bs.read_uint32()
                if self.HEADER_version != 60 :
                    F1 = bs.read_uint32()
                while ReadUInt(bs, bs.tell()) !=1346980931:
                    bs.skip(1)
            magic = bs.read_uint32()
            TotalData = bs.read_uint32()
            NumFrames = bs.read_float32()
            numClips = bs.read_uint32()
            numStrings = bs.read_uint32()
            numData = bs.read_uint32()
            hash = None
            if self.HEADER_version != 99 :
                hash = bs.read_uint64()
                hash = bs.read_uint64()
            clipDataOffs = bs.read_uint64()
            propertiesOffs = bs.read_uint64()
            fnDataOffs = bs.read_uint64()
            namesOffs = bs.read_uint64()
            namesOffs2 = None
            if self.HEADER_version == 85 :
                namesOffs2 = bs.read_uint64()
            namesOffsExtra = []
            if self.HEADER_version == 60 :
                for ni in range(5):
                    namesOffsExtra.append(bs.read_uint64())
            else:
                for ni in range(4):
                    namesOffsExtra.append(bs.read_uint64())
            print(namesOffsExtra)
            unicodeNamesOffs = bs.read_uint64()
            endClipStructsOffs = bs.read_uint64()
            print("clipIdx: %s,  unicodeNamesOffs: %s,  endClipStructsOffs: %s"%(clipIdx, unicodeNamesOffs, endClipStructsOffs))
            bs.seek(clipDataOffs)
            
            Type = 6

            # Function Data
            if numData > 0 :
                bs.seek(fnDataOffs)
                if self.HEADER_version == 85:
                    for cfdi in range(numData):
                        # cprint("[%s]Function Data %s"%(cfdi, hex(bs.tell() + self.start)), 'green', 'on_red')
                        # if(Type == 6 and (exists(parentof(this).Data[1]))):
                        #     bs.skip(4)
                        Value = bs.read_float32()
                        print(cfdi, "Value", Value)
                        bs.skip(4)
                        SubDataType = bs.read_uint64()
                        print(cfdi, "SubDataType", SubDataType)
                        if (SubDataType == 2):
                            bs.skip(4)
                            Rate = bs.read_float32()
                        elif (((SubDataType == 3) or (SubDataType == 1))):
                            ID = bs.read_int32()
                            if(Type != 6):
                                pos2 = bs.tell()
                                if(namesOffsExtra[1] + ID < bs.get_buffer_length()):
                                    bs.seek(namesOffsExtra[1] + ID)
                                    print(cfdi, "namesOffsExtra[1]:", namesOffsExtra[1])
                                    if(ID < bs.get_buffer_length() and ID > 1 and ReadUByte(bs, bs.tell()) != 0 and ReadUByte(bs, bs.tell()-1)==0 and len(ReadString(bs))>2):
                                        IDName_FnNames = ReadString(bs)
                                        print(cfdi, "IDName_FnNames:", IDName_FnNames)
                                    bs.seek(unicodeNamesOffs + ID*2)
                                    print(cfdi, "unicodeNamesOffs:", unicodeNamesOffs)
                                    if(ID < bs.get_buffer_length() and ID > 1 and ReadUByte(bs, bs.tell()) != 0 and ReadUByte(bs, bs.tell()-2)==0):
                                        IDName_clipNames = readUnicodeStringAt(bs, bs.tell())
                                        print(cfdi, "IDName_clipNames:", IDName_clipNames)
                                bs.seek(pos2 + 4)
                        elif (SubDataType == 5):
                            Angle = bs.read_float64()
                            AngleID = bs.read_int32()
                            bs.skip(-4)
                        else:
                            UnkDataValue = bs.read_uint64()
                            bs.skip(4)
                        bs.skip(16)
                    pass
                else:
                    pass


            print("clipIdx: %s,  endClipStructsOffs: %s"%(clipIdx, endClipStructsOffs))
            bs.seek(endClipStructsOffs)
            while (ReadUInt64(bs, bs.tell()) == 0):
                bs.skip(8)
            nextStructOffs = bs.read_uint64()
            
            print("clipIdx: %s,  nextStructOffs: %s"%(clipIdx, nextStructOffs))
            bs.seek(nextStructOffs)

    # def checkIfSyncMot(self, other):
    #     return (self.frameCount == other.frameCount)
    #     '''if self.frameCount == other.frameCount:
    #         boneNames = [self.motlist.bones[kfBone.boneIndex].name.lower() for kfBone in self.kfBones]
    #         otherBoneNames = [other.motlist.bones[kfBone.boneIndex].name.lower() for kfBone in other.kfBones]
    #         counter = 0
    #         for boneName in boneNames:
    #             if boneName in otherBoneNames:
    #                 counter += 1
    #         return (counter / len(boneNames) < 0.25)'''
                
    
    # def readFrame(self, ftype, flags, unpacks):
    #     bs = self.bs
    #     compression = flags & 0xFF000
    #     if ftype=="pos" or ftype=="scl":
    #         defScaleVec = NoeVec3((fDefaultMeshScale, fDefaultMeshScale, fDefaultMeshScale))
    #         if compression == 0x00000:
    #             output = NoeVec3((bs.readFloat(), bs.readFloat(), bs.readFloat())) * defScaleVec
    #         elif compression == 0x20000:
    #             rawVec = readPackedBitsVec3(bs.readUShort(), 5)
    #             if self.version <= 65:
    #                 output = NoeVec3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.z, unpacks.max.y * rawVec[2] + unpacks.min.z)) * defScaleVec
    #             else:
    #                 output = NoeVec3((unpacks.max.x * rawVec[0] + unpacks.max.w, unpacks.max.y * rawVec[1] + unpacks.min.x, unpacks.max.z * rawVec[2] + unpacks.min.y)) * defScaleVec
    #         elif compression == 0x24000:
    #             x = y = z = unpacks.max.x * convertBits(bs.readUShort(), 16) + unpacks.min.x
    #             output = NoeVec3((x, y, z)) * defScaleVec
    #         elif compression == 0x44000:
    #             x = y = z = unpacks.max.x * bs.readFloat() + unpacks.min.x
    #             output = NoeVec3((x, y, z)) * defScaleVec
    #         elif compression == 0x40000 or (compression == 0x30000 and self.version <= 65):
    #             rawVec = readPackedBitsVec3(bs.readUInt(), 10)
    #             if self.version <= 65:
    #                 output = NoeVec3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.y, unpacks.max.z * rawVec[2] + unpacks.min.z)) * defScaleVec
    #             else:
    #                 output = NoeVec3((unpacks.max.x * rawVec[0] + unpacks.max.w, unpacks.max.y * rawVec[1] + unpacks.min.x, unpacks.max.z * rawVec[2] + unpacks.min.y)) * defScaleVec
    #         elif compression == 0x70000:
    #             rawVec = readPackedBitsVec3(bs.readUInt64(), 21)
    #             output = NoeVec3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.y, unpacks.max.z * rawVec[2] + unpacks.min.z)) * defScaleVec
    #         elif compression == 0x80000:
    #             rawVec = readPackedBitsVec3(bs.readUInt64(), 21)
    #             output = NoeVec3((unpacks.max.x * rawVec[0] + unpacks.max.w, unpacks.max.y * rawVec[1] + unpacks.min.x, unpacks.max.z * rawVec[2] + unpacks.min.y)) * defScaleVec
    #         elif (compression == 0x31000 and self.version <= 65) or (compression == 0x41000 and self.version >= 78): #LoadVector3sXAxis
    #             output = NoeVec3((bs.readFloat(), unpacks.max.y, unpacks.max.z)) * defScaleVec
    #         elif (compression == 0x32000 and self.version <= 65) or (compression == 0x42000 and self.version >= 78): #LoadVector3sYAxis
    #             output = NoeVec3((unpacks.max.x, bs.readFloat(), unpacks.max.z)) * defScaleVec
    #         elif (compression == 0x33000 and self.version <= 65) or (compression == 0x43000 and self.version >= 78): #LoadVector3sZAxis
    #             output = NoeVec3((unpacks.max.x, unpacks.max.y, bs.readFloat())) * defScaleVec
    #         elif compression == 0x21000:
    #             output = NoeVec3((unpacks.max.x * convertBits(bs.readUShort(), 16) + unpacks.max.y, unpacks.max.z, unpacks.max.w)) * defScaleVec
    #         elif compression == 0x22000:
    #             output = NoeVec3((unpacks.max.y, unpacks.max.x * convertBits(bs.readUShort(), 16) + unpacks.max.z, unpacks.max.w)) * defScaleVec
    #         elif compression == 0x23000:
    #             output = NoeVec3((unpacks.max.y, unpacks.max.z, unpacks.max.x * convertBits(bs.readUShort(), 16) + unpacks.max.w)) * defScaleVec
    #         else:
    #             print("Unknown", "Translation" if ftype=="pos" else "Scale", "type:", "0x"+'{:02X}'.format(compression))
    #             output = NoeVec3((0,0,0)) if ftype=="pos" else NoeVec3((100,100,100))
    #     elif ftype=="rot":
    #         if compression == 0x00000: #LoadQuaternionsFull
    #             output = NoeQuat((bs.readFloat(), bs.readFloat(), bs.readFloat(), bs.readFloat())).transpose()
    #         elif compression == 0xB0000 or compression == 0xC0000: #LoadQuaternions3Component
    #             #rawVec = [bs.readFloat(), bs.readFloat(), bs.readFloat()]
    #             #output = NoeQuat((rawVec[0], rawVec[1], rawVec[2], wRot(rawVec))).transpose()
    #             output = NoeQuat3((bs.readFloat(), bs.readFloat(), bs.readFloat())).toQuat().transpose()
    #         elif compression == 0x20000: #//LoadQuaternions5Bit RE3
    #             rawVec = readPackedBitsVec3(bs.readUShort(), 5)
    #             output = NoeQuat3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.y, unpacks.max.z * rawVec[2] + unpacks.min.z)).toQuat().transpose()
    #         elif compression == 0x21000:
    #             output = NoeQuat3((unpacks.max.x * convertBits(bs.readUShort(), 16) + unpacks.max.y, 0, 0)).toQuat().transpose()
    #         elif compression == 0x22000:
    #             output = NoeQuat3((0, unpacks.max.x * convertBits(bs.readUShort(), 16) + unpacks.max.y, 0)).toQuat().transpose()
    #         elif compression == 0x23000:
    #             output = NoeQuat3((0, 0, unpacks.max.x * convertBits(bs.readUShort(), 16) + unpacks.max.y)).toQuat().transpose()
    #         elif compression == 0x30000 and self.version >= 78: #LoadQuaternions8Bit RE3
    #             rawVec = [convertBits(bs.readUByte(), 8), convertBits(bs.readUByte(), 8), convertBits(bs.readUByte(), 8)]
    #             output = NoeQuat3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.y, unpacks.max.z * rawVec[2] + unpacks.min.z)).toQuat().transpose()
    #         elif compression == 0x30000:
    #             rawVec = readPackedBitsVec3(bs.readUInt(), 10)
    #             output = NoeQuat3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.y, unpacks.max.z * rawVec[2] + unpacks.min.z)).toQuat().transpose()
    #         elif compression == 0x31000 or compression == 0x41000:
    #             output = NoeQuat3((bs.readFloat(), 0, 0)).toQuat().transpose()
    #         elif compression == 0x32000 or compression == 0x42000:
    #             output = NoeQuat3((0, bs.readFloat(), 0)).toQuat().transpose()
    #         elif compression == 0x33000 or compression == 0x43000:
    #             output = NoeQuat3((0, 0, bs.readFloat())).toQuat().transpose()
    #         elif compression == 0x40000: #LoadQuaternions10Bit RE3
    #             rawVec = readPackedBitsVec3(bs.readUInt(), 10)
    #             output = NoeQuat3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.y, unpacks.max.z * rawVec[2] + unpacks.min.z)).toQuat().transpose()
    #         elif compression == 0x50000 and self.version <= 65: #LoadQuaternions16Bit RE2
    #             rawVec = [convertBits(bs.readUShort(), 16), convertBits(bs.readUShort(), 16), convertBits(bs.readUShort(), 16)]
    #             output = NoeQuat3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.y, unpacks.max.z * rawVec[2] + unpacks.min.z)).toQuat().transpose()
    #         elif compression == 0x50000: #LoadQuaternions13Bit RE3
    #             rawBytes = [bs.readUByte(), bs.readUByte(), bs.readUByte(), bs.readUByte(), bs.readUByte()]
    #             retrieved = (rawBytes[0] << 32) | (rawBytes[1] << 24) | (rawBytes[2] << 16) | (rawBytes[3] << 8) | (rawBytes[4] << 0)
    #             rawVec = readPackedBitsVec3(retrieved, 13)
    #             output = NoeQuat3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.y, unpacks.max.z * rawVec[2] + unpacks.min.z)).toQuat().transpose()
    #         elif compression == 0x60000: #LoadQuaternions16Bit RE3
    #             #output = NoeQuat((0,0,0,1))
    #             rawVec = [convertBits(bs.readUShort(), 16), convertBits(bs.readUShort(), 16), convertBits(bs.readUShort(), 16)]
    #             output = NoeQuat3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.y, unpacks.max.z * rawVec[2] + unpacks.min.z)).toQuat().transpose()
    #         elif (compression == 0x70000 and self.version <= 65) or (compression == 0x80000 and self.version >= 78): #LoadQuaternions21Bit RE2 and LoadQuaternions21Bit RE3
    #             rawVec = readPackedBitsVec3(bs.readUInt64(), 21)
    #             output = NoeQuat3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.y, unpacks.max.z * rawVec[2] + unpacks.min.z)).toQuat().transpose()
    #         elif compression == 0x70000 and self.version >= 78: #LoadQuaternions18Bit RE3
    #             rawBytes = [bs.readUByte(), bs.readUByte(), bs.readUByte(), bs.readUByte(), bs.readUByte(), bs.readUByte(), bs.readUByte()]
    #             retrieved = (rawBytes[0] << 48) | (rawBytes[1] << 40) | (rawBytes[2] << 32) | (rawBytes[3] << 24) | (rawBytes[4] << 16) | (rawBytes[5] << 8) | (rawBytes[6] << 0)
    #             rawVec = readPackedBitsVec3(retrieved, 18)
    #             output = NoeQuat3((unpacks.max.x * rawVec[0] + unpacks.min.x, unpacks.max.y * rawVec[1] + unpacks.min.y, unpacks.max.z * rawVec[2] + unpacks.min.z)).toQuat().transpose()
    #         else:
    #             print("Unknown Rotation type:", "0x"+'{:02X}'.format(compression))
    #             output = NoeQuat((0,0,0,1))
    #     return output
            
    # # Used a lot for merging+moving skeletons of animations and meshes together:
    # def readBoneHeaders(self):
    #     bs = self.bs
    #     boneHdrOffs = 0
    #     if self.offsToBoneHdrOffs:
    #         bs.seek(self.offsToBoneHdrOffs)
    #         self.boneHdrOffset = bs.readUInt64()
    #         count = bs.readUInt64()
    #         if self.boneHdrOffset and count == self.boneCount:
    #             boneHdrOffs = self.boneHdrOffset
    #     if boneHdrOffs:
    #         bs.seek(boneHdrOffs)
    #         for i in range(count):
    #             bs.seek(self.boneHdrOffset+80*i)
    #             boneName = readUnicodeStringAt(bs, bs.readUInt64())
    #             #boneName = self.motlist.meshBones[i].name if i < len(self.motlist.meshBones) else boneName #SF6 facial anims test
    #             parentOffset = bs.readUInt64()
    #             parentIndex = int((parentOffset-self.boneHdrOffset)/80) if parentOffset else -1
    #             bs.seek(16,1)
    #             translation = NoeVec4((bs.readFloat(), bs.readFloat(), bs.readFloat(), bs.readFloat()))
    #             quat = NoeQuat((bs.readFloat(), bs.readFloat(), bs.readFloat(), bs.readFloat())).transpose()
    #             index = bs.readUInt()
    #             boneHash = bs.readUInt()
    #             mat = quat.toMat43()
    #             mat[3] = translation.toVec3() * fDefaultMeshScale
    #             self.boneHeaders.append(BoneHeader(name=boneName, pos=translation, rot=quat, index=index, parentIndex=parentIndex, hash=boneHash, mat=mat))
    #         self.motlist.boneHeaders = self.motlist.boneHeaders or self.boneHeaders
    #     elif self.motlist.boneHeaders:
    #         self.boneHeaders = self.motlist.boneHeaders
    #     elif not self.motlist.searchedForBoneHeaders:
    #         self.motlist.findBoneHeaders()
    #     else:
    #         print("Failed to find bone headers:", self.name)
    #         return 0
            
    #     self.bones = []
    #     if not dialogOptions.dialog or not dialogOptions.dialog.args.get("mesh"):
            
    #         meshBoneNames = [bone.name.lower() for bone in self.motlist.meshBones]
    #         motlistBoneNames = [bone.name.lower() for bone in self.motlist.bones]
            
    #         for i, boneHeader in enumerate(self.boneHeaders):
    #             if not meshBoneNames or boneHeader.name.lower() in meshBoneNames: #always use additive animations when loading onto meshes
    #                 bone = NoeBone(len(self.bones), boneHeader.name, boneHeader.mat, self.boneHeaders[boneHeader.parentIndex].name if boneHeader.parentIndex != -1 else None, boneHeader.parentIndex)
    #                 self.bones.append(bone)
            
    #         selfBoneNames = [bone.name.lower() for bone in self.bones]
    #         addedBones = []
    #         for i, bone in enumerate(self.bones):
    #             if bone.parentName and bone.parentName.lower() in motlistBoneNames:
    #                 bone.parentIndex = motlistBoneNames.index(bone.parentName.lower())
    #             if bone.name.lower() not in motlistBoneNames:
    #                 bone.index = len(self.motlist.bones)
    #                 self.motlist.bones.append(bone)
    #                 motlistBoneNames.append(bone.name.lower())
    #                 addedBones.append(bone)
    #         for b, bone in enumerate(self.bones):
    #             if bone.parentIndex != -1 and bone.parentName.lower() in motlistBoneNames:
    #                 mat = self.boneHeaders[b].mat
    #                 bone.setMatrix(mat * self.motlist.bones[motlistBoneNames.index(bone.parentName.lower())].getMatrix())
    #                 '''if bone in addedBones:
    #                     mat = NoeMat43() #remove posed rotation from anim skeleton, and relocate bone to merged parent bone
    #                     mat[3] = self.boneHeaders[b].pos.toVec3()'''
    #                 '''if bone in addedBones:
    #                     childBones = getChildBones(bone, self.motlist.bones, True)
    #                     childMats = []
    #                     for childBone in childBones:
    #                         #print("moving child", childBone.name)
    #                         oldIndex = selfBoneNames.index(childBone.name.lower())
    #                         childMats.append(self.boneHeaders[oldIndex].mat * self.bones[selfBoneNames.index(childBone.parentName.lower())].getMatrix())'''

    #                 '''if bone in addedBones:
    #                     for c, childBone in enumerate(childBones):
    #                         #childBone.setMatrix(childMats[c] * self.motlist.bones[motlistBoneNames.index(childBone.parentName.lower())].getMatrix())
    #                         print("moving child", childBone.name)
    #                         childBone.setMatrix(NoeMat43())'''
                
    # def read(self):
    #     bs = self.bs
        
    #     if not self.boneHeaders:
    #         self.readBoneHeaders()
        
    #     bnClipSz = 24 if self.version==65 else 16 if self.version==43 else 12
    #     for i in range(self.boneClipCount):
    #         bs.seek(self.boneClipHdrOffset+bnClipSz*i)
    #         #print(i, "bnCLipHdr at", bs.tell()+self.start)
    #         if self.version == 65:
    #             index = bs.readUShort()
    #             trackFlags = bs.readUShort()
    #             boneHash = bs.readUInt()
    #             bs.seek(8,1)
    #             trackHeaderOffset = bs.readUInt64()
    #         else:
    #             index = bs.readUShort()
    #             trackFlags = bs.readUShort()
    #             boneHash = bs.readUInt()
    #             if  self.version == 43:
    #                 trackHeaderOffset = bs.readUInt64()
    #             else:
    #                 trackHeaderOffset = bs.readUInt()
    #         self.boneClipHeaders.append(BoneClipHeader(boneIndex=index, trackFlags=trackFlags, boneHash=boneHash, trackHeaderOffset=trackHeaderOffset))
            
    #     skipToNextLine(bs)
    #     self.boneClips = []
    #     for i in range(self.boneClipCount):
    #         boneClipHdr = self.boneClipHeaders[i]
    #         #if self.boneHeaders[boneClipHdr.boneIndex].name in 
    #         #if (i == 0 and self.boneHeaders[boneClipHdr.boneIndex].name != self.motlist.bones[0].name):
    #         #	print(self.name, "Ignoring all keyframes for ", self.boneHeaders[boneClipHdr.boneIndex].name)
    #         #	continue
    #         tracks = {"pos": None, "rot": None, "scl": None }
    #         bs.seek(boneClipHdr.trackHeaderOffset)
    #         for t in range(3):
    #             if boneClipHdr.trackFlags & (1 << t):
    #                 flags = bs.readUInt()
    #                 keyCount = bs.readUInt()
    #                 frameRate = maxFrame = 0
    #                 if self.version >= 78:
    #                     frameIndOffs = bs.readUInt()
    #                     frameDataOffs = bs.readUInt()
    #                     unpackDataOffs = bs.readUInt()
    #                 else:
    #                     frameRate = float(bs.readUInt())
    #                     maxFrame = bs.readFloat()
    #                     frameIndOffs = bs.readUInt64()
    #                     frameDataOffs = bs.readUInt64()
    #                     unpackDataOffs = bs.readUInt64()
    #                 newTrack = BoneTrack(flags=flags, keyCount=keyCount, frameRate=frameRate, maxFrame=maxFrame, frameIndOffs=frameIndOffs, frameDataOffs=frameDataOffs, unpackDataOffs=unpackDataOffs)
    #                 if (boneClipHdr.trackFlags & (1)) and not tracks.get("pos"): 
    #                     tracks["pos"] = newTrack
    #                 elif (boneClipHdr.trackFlags & (1 << 1)) and not tracks.get("rot"): 
    #                     tracks["rot"] = newTrack
    #                 elif (boneClipHdr.trackFlags & (1 << 2)) and not tracks.get("scl"): 
    #                     tracks["scl"] = newTrack
    #         if i == 0 and dialogOptions.dialog and dialogOptions.dialog.pak and self.boneHeaders[boneClipHdr.boneIndex].name != dialogOptions.dialog.pak.bones[0].name:
    #         #if i == 0 and self.boneHeaders[boneClipHdr.boneIndex].name != "root":
    #             print(self.name, ": Ignoring all keyframes for ", self.boneHeaders[boneClipHdr.boneIndex].name)
    #             tracks["pos"] = tracks["rot"] = tracks["scl"] = None #remove local root bone translations/rotations for mounted animations like facials 
    #         elif dialogOptions.doForceCenter and (self.boneHeaders[boneClipHdr.boneIndex].parentIndex == 0 or i == 0):
    #             print(self.name, ": Ignoring position keyframes for ", self.boneHeaders[boneClipHdr.boneIndex].name)
    #             tracks["pos"] = None
    #         self.boneClips.append(tracks)
            
    #     for i, boneClip in enumerate(self.boneClips):
    #         motlistBoneIndex = self.motlist.boneHashes.get(self.boneClipHeaders[i].boneHash)
    #         if motlistBoneIndex != None:
    #             kfBone = NoeKeyFramedBone(motlistBoneIndex)
    #             for ftype in ["pos", "rot", "scl"]:
    #                 fHeader = boneClip.get(ftype)
    #                 if fHeader:
    #                     keyCompression = fHeader.flags >> 20
    #                     keyReadFunc = bs.readUInt if keyCompression==5 else bs.readUByte if keyCompression==2 else bs.readUShort
    #                     bs.seek(fHeader.frameIndOffs)
    #                     keyTimes = []
    #                     for k in range(fHeader.keyCount):
    #                         keyTimes.append(keyReadFunc() if fHeader.frameIndOffs else 0)
    #                     if fHeader.unpackDataOffs:
    #                         bs.seek(fHeader.unpackDataOffs)
    #                         unpackMax = UnpackVec(x=bs.readFloat(), y=bs.readFloat(), z=bs.readFloat(), w=bs.readFloat())
    #                         unpackMin = UnpackVec(x=bs.readFloat(), y=bs.readFloat(), z=bs.readFloat(), w=bs.readFloat())
    #                     else:
    #                         unpackMax = unpackMin = UnpackVec(x=0, y=0, z=0, w=0)
    #                     unpackValues = Unpacks(max=unpackMax, min=unpackMin)
    #                     frames = []
    #                     bs.seek(fHeader.frameDataOffs)
    #                     for f in range(fHeader.keyCount):
    #                         frame = self.readFrame(ftype, fHeader.flags, unpackValues)
    #                         if ftype == "scl":
    #                             frame /= 100
    #                         kfValue = NoeKeyFramedValue(keyTimes[f], frame)
    #                         frames.append(kfValue)
    #                     if ftype == "pos": # and self.motlist.bones[motlistBoneIndex].parentIndex != 0:#kfBoneNames:
    #                         kfBone.setTranslation(frames, noesis.NOEKF_TRANSLATION_VECTOR_3)
    #                     elif ftype == "rot":
    #                         kfBone.setRotation(frames, noesis.NOEKF_ROTATION_QUATERNION_4)
    #                     elif ftype == "scl":
    #                         kfBone.setScale(frames, noesis.NOEKF_SCALE_VECTOR_3)
    #             self.kfBones.append(kfBone)
    #     motEnd = bs.tell()
        
    #     #bs.seek(0)
    #     #self.bs = NoeBitStream(bs.readBytes(motEnd))
    #     #self.anim = NoeKeyFramedAnim(self.name, self.motlist.bones, self.kfBones, 1)


class MotListFileHeader:
    def __init__(self, data: bytes):
        pass

class MotListFile:
    def __init__(self, data: bytes, path=""):
        bs = BinaryStream(data)
        self.bs = bs
        self.path = path
        self.bones = []
        self.boneHashes = {}
        self.boneHeaders = []
        self.anims = []
        self.mots = []
        self.meshBones = []
        self.searchedForBoneHeaders = False
        self.totalFrames = 0
        self.version = bs.readInt()
        self.ID = bs.read(4)
        self.header_padding = bs.read_uint64()
        pointersOffset = bs.readUInt64()
        motionIDsOffset = bs.readUInt64() #UnknownPointer000
        motListNameOffs = bs.readUInt64()
        self.name = readUnicodeStringAt(bs, motListNameOffs)
        if self.version != 60:
            bs.seek(8, 1) #UnknownPointer001
            
        numOffsets = bs.readUInt()
        bs.seek(pointersOffset)
        self.motionIDs = []
        self.pointers = []
        for i in range(numOffsets):
            bs.seek(pointersOffset + i * 8)
            motAddress = bs.readUInt64()
            if (motAddress and motAddress not in self.pointers and ReadUInt(bs, motAddress + 4) == 544501613):  # 'mot'
                self.pointers.append(motAddress)
                bs.seek(motAddress)
                mot = MotFile(bs.read(bs.get_buffer_length() - bs.tell()), self, motAddress)
                self.mots.append(mot)
                # print(mot.name)
                if mot.name == "pl0800_YMT_ComboA_3":
                    mot.export()
                    pass

def exportMotlistFile(path):
    meshBuffer = None
    with open(path, "rb") as itaOpened:
        meshBuffer = itaOpened.read(-1)
        motlistFile = MotListFile(meshBuffer)


exportMotlistFile(motlist_file_path)
