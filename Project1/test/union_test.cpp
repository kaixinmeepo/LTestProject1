#include "union_test.h"
#include <limits.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include "test.h"

namespace UNION_TEST {
    namespace TestEnv1 {
        // 可以用结构的位域分割共享空间，且相互不影响，以下为测试例子
        void Test() {
            union
            {
                struct
                {
                    unsigned int count : 31;
                    unsigned int is_request : 1;
                };
                unsigned int id;
            } test;
            test.id = 0;
            std::cout << "test:" << test.id << " " << test.count << " " << test.is_request << std::endl;
            test.count = UINT_MAX;
            ++test.count;
            std::cout << "test:" << test.id << " " << test.count << " " << test.is_request << std::endl;
        }
    }
    namespace TestEnv2 {
        struct attack_msg {
            struct FixData {
                int attack_physic = 0;  //物理攻击
                int attack_magic = 0;  //法术攻击
            };
            FixData fix_data_;
        private:
            enum VARIANT_DATA_INDEX {
                Index_Master_PhyAtk              = 0,
                Index_Master_MagAtk              = 1,
                Index_Master_PhyDef              = 2,
                Index_Master_MagDef              = 3,
                Index_Master_HP                  = 4,
                Index_Master_maxHP               = 5,
                Index_Master_CritRate            = 6,
                Index_Master_CritResRate         = 7,
                Index_Master_CritRatio           = 8,
                Index_Master_WildBlowRate        = 9,
                Index_Master_WildBlowResRate     = 10,
                Index_Master_WildBlowRatio       = 11,
                Index_Master_TruthDmg            = 12,
                Index_Master_BleedDmg            = 13,
                Index_Master_FireDmg             = 14,
                Index_Master_FireDmgAddRatio     = 15,
                Index_Master_FireDmgReduRatio    = 16,
                Index_Master_IceDmg              = 17,
                Index_Master_IceDmgAddRatio      = 18,
                Index_Master_IceDmgReduRatio     = 19,
                Index_Master_LightDmg            = 20,
                Index_Master_LightDmgAddRatio    = 21,
                Index_Master_LightDmgReduRatio   = 22,
                Index_Master_ThunderDmg          = 23,
                Index_Master_ThunderDmgAddRatio  = 24,
                Index_Master_ThunderDmgReduRatio = 25,
                Index_Master_DarkDmg             = 26,
                Index_Master_DarkDmgAddRatio     = 27,
                Index_Master_DarkDmgReduRatio    = 28,
                Index_Master_WindDmg             = 29,
                Index_Master_WindDmgAddRatio     = 30,
                Index_Master_WindDmgReduRatio    = 31,
                Index_Master_PoisonDmg           = 32,
                Index_Master_PoisonDmgAddRatio   = 33,
                Index_Master_PoisonDmgReduRatio  = 34,
                Index_Master_EnergySheild        = 35,
                Index_Master_maxEnergySheild     = 36,
                MAX_INDEX,
            };
            enum {
                VARIANT_HEADER_SIZE = (MAX_INDEX + 31) / 32,
            };
            uint32_t variant_data_size_ = 0;
            uint32_t variant_header_[VARIANT_HEADER_SIZE] = { 0 };
            int32_t variant_data_arry_[MAX_INDEX] = { 0 };
            template<typename T>
            void SetVariantData(int index, T& value) {
                if (index < 0 || index >= MAX_INDEX) {
                    return;
                }
                int32_t i_value = *(int32_t*)(&value);
                if (i_value != 0) {
                    variant_data_arry_[index] = i_value;
                    variant_header_[index / 32] |= 1 << (index % 32);
                    ++variant_data_size_;
                }
            }
            template<typename T>
            T GetVariantData(int index, T) {
                if (index < 0 || index >= MAX_INDEX) {
                    return -1;
                }
                return *(T*)(&variant_data_arry_[index]);
            }
            int GetVariantValueSize() {
                return (variant_data_size_ + VARIANT_HEADER_SIZE) * sizeof(int32_t);
            }
            void SerializeVariantData(void* buf) {
                if (variant_data_size_ <= 0) {
                    return;
                }
                auto buf_data = (int32_t*)(buf);
                uint32_t data_index = 0, buf_index = VARIANT_HEADER_SIZE, tmp_header = 0;
                for (int i = 0; i < VARIANT_HEADER_SIZE; ++i) {
                    buf_data[i] = variant_header_[i];
                    //tmp_header = variant_header_[i];
                    //for (int j = 0; tmp_header; tmp_header &= tmp_header - 1) {
                    //for (int j = 0; tmp_header; tmp_header &= ~(1 << j)) {
                        //j = __builtin_ctz(tmp_header);
                    for (int j = 0; j < sizeof(uint32_t) * 8; ++j) {
                        if (buf_index >= VARIANT_HEADER_SIZE + variant_data_size_) {
                            return;
                        }
                        data_index = i * 32 + j;
                        if (data_index >= MAX_INDEX) {
                            return;
                        }
                        if (variant_header_[i] & ((uint32_t)1 << j)) {
                            buf_data[buf_index] = variant_data_arry_[data_index];
                            ++buf_index;
                        }
                    }
                }
            }
            void UnSerializeVariantData(void* buf, size_t buf_size) {
                if (buf_size < sizeof(variant_header_) + sizeof(int32_t)) {
                    return;
                }
                memcpy(variant_header_, buf, VARIANT_HEADER_SIZE * sizeof(uint32_t));
                auto buf_data = (int32_t*)(buf);
                uint32_t data_index = 0, buf_index = VARIANT_HEADER_SIZE, tmp_header = 0;
                for (int i = 0; i < VARIANT_HEADER_SIZE; ++i) {
                    //tmp_header = variant_header_[i];
                    //for (int j = 0; tmp_header; tmp_header &= tmp_header - 1) {
                    //for (int j = 0; tmp_header; tmp_header &= ~(1 << j)) {
                    //    j = __builtin_ctz(tmp_header);
                    for (int j = 0; j < sizeof(uint32_t)*8; ++j) {
                        data_index = i * 32 + j;
                        if (data_index >= MAX_INDEX) {
                            variant_data_size_ = buf_index - VARIANT_HEADER_SIZE;
                            return;
                        }
                        if (variant_header_[i] & ((uint32_t)1 << j)) {
                            variant_data_arry_[data_index] = buf_data[buf_index];
                            ++buf_index;
                        }
                    }
                }
                variant_data_size_ = buf_index - VARIANT_HEADER_SIZE;
            }

        public:
#define GET_SET_DATA(prop, tp) \
            void Set##prop(tp value) { SetVariantData(Index_##prop, value); } \
            int Get##prop() { return GetVariantData(Index_##prop, tp(0)); }
            GET_SET_DATA(Master_PhyAtk, int)
            GET_SET_DATA(Master_MagAtk, float)
            GET_SET_DATA(Master_PhyDef, int)
            GET_SET_DATA(Master_MagDef, int)
            GET_SET_DATA(Master_HP, int)
            GET_SET_DATA(Master_maxHP, int)
            GET_SET_DATA(Master_CritRate, int)
            GET_SET_DATA(Master_CritResRate, int)
            GET_SET_DATA(Master_CritRatio, int)
            GET_SET_DATA(Master_WildBlowRate, int)
            GET_SET_DATA(Master_WildBlowResRate, int)
            GET_SET_DATA(Master_WildBlowRatio, int)
            GET_SET_DATA(Master_TruthDmg, int)
            GET_SET_DATA(Master_BleedDmg, int)
            GET_SET_DATA(Master_FireDmg, int)
            GET_SET_DATA(Master_FireDmgAddRatio, int)
            GET_SET_DATA(Master_FireDmgReduRatio, int)
            GET_SET_DATA(Master_IceDmg, int)
            GET_SET_DATA(Master_IceDmgAddRatio, int)
            GET_SET_DATA(Master_IceDmgReduRatio, int)
            GET_SET_DATA(Master_LightDmg, int)
            GET_SET_DATA(Master_LightDmgAddRatio, int)
            GET_SET_DATA(Master_LightDmgReduRatio, int)
            GET_SET_DATA(Master_ThunderDmg, int)
            GET_SET_DATA(Master_ThunderDmgAddRatio, int)
            GET_SET_DATA(Master_ThunderDmgReduRatio, int)
            GET_SET_DATA(Master_DarkDmg, int)
            GET_SET_DATA(Master_DarkDmgAddRatio, int)
            GET_SET_DATA(Master_DarkDmgReduRatio, int)
            GET_SET_DATA(Master_WindDmg, int)
            GET_SET_DATA(Master_WindDmgAddRatio, int)
            GET_SET_DATA(Master_WindDmgReduRatio, int)
            GET_SET_DATA(Master_PoisonDmg, int)
            GET_SET_DATA(Master_PoisonDmgAddRatio, int)
            GET_SET_DATA(Master_PoisonDmgReduRatio, int)
            GET_SET_DATA(Master_EnergySheild, int)
            GET_SET_DATA(Master_maxEnergySheild, int)
#undef GET_SET_DATA
            size_t GetBufSize() { return sizeof(FixData) + GetVariantValueSize(); }
            void Serialize(void* buf, size_t buf_size) {
                *(FixData*)buf = fix_data_;
                SerializeVariantData(buf + sizeof(FixData));
            }
            void UnSerialize(void* buf, size_t buf_size) {
                fix_data_ = *(FixData*)buf;
                UnSerializeVariantData((char*)buf + sizeof(FixData), buf_size - sizeof(FixData));
            }
            void Dump() {
                printf("size:%d\n", variant_data_size_);
                for (auto i = 0; i < VARIANT_HEADER_SIZE; ++i) {
                    printf("header%d:%d\n", i, variant_header_[i]);
                }
                for (auto i = 0; i < MAX_INDEX; ++i) {
                    printf("data%d:%d\n", i, variant_data_arry_[i]);
                }
            }
        };
        void Test() {
            CalcTime time_calc;
            attack_msg msg;
            msg.SetMaster_PhyAtk(-1);
            msg.SetMaster_MagAtk(1);
            msg.SetMaster_PhyDef(2);
            msg.SetMaster_MagDef(3);
            msg.SetMaster_HP(4);
            msg.SetMaster_maxHP(5);
            msg.SetMaster_CritRate(6);
            msg.SetMaster_CritResRate(7);
            msg.SetMaster_CritRatio(8);
            msg.SetMaster_WildBlowRate(9);
            msg.SetMaster_WildBlowResRate(10);
            msg.SetMaster_WildBlowRatio(11);
            msg.SetMaster_TruthDmg(12);
            msg.SetMaster_BleedDmg(13);
            msg.SetMaster_FireDmg(14);
            msg.SetMaster_FireDmgAddRatio(15);
            msg.SetMaster_FireDmgReduRatio(16);
            msg.SetMaster_IceDmg(17);
            msg.SetMaster_IceDmgAddRatio(18);
            msg.SetMaster_IceDmgReduRatio(19);
            msg.SetMaster_LightDmg(20);
            msg.SetMaster_LightDmgAddRatio(21);
            msg.SetMaster_LightDmgReduRatio(22);
            msg.SetMaster_ThunderDmg(23);
            msg.SetMaster_ThunderDmgAddRatio(24);
            msg.SetMaster_ThunderDmgReduRatio(25);
            msg.SetMaster_DarkDmg(26);
            msg.SetMaster_DarkDmgAddRatio(27);
            msg.SetMaster_DarkDmgReduRatio(28);
            msg.SetMaster_WindDmg(29);
            msg.SetMaster_WindDmgAddRatio(30);
            msg.SetMaster_WindDmgReduRatio(31);
            msg.SetMaster_PoisonDmg(32);
            msg.SetMaster_PoisonDmgAddRatio(33);
            msg.SetMaster_PoisonDmgReduRatio(34);
            msg.SetMaster_EnergySheild(35);
            msg.SetMaster_maxEnergySheild(36);
            //std::cout << msg.GetMaster_PhyAtk() << std::endl;
            //std::cout << msg.GetMaster_MagAtk() << std::endl;
            size_t buf_size = msg.GetBufSize();
            void* buf_data = operator new(buf_size);
            time_calc.Begin();
            msg.Serialize(buf_data, buf_size);

            attack_msg msg2;
            msg2.UnSerialize(buf_data, buf_size);
            time_calc.End();
            time_calc.Dump();
            delete buf_data;
            //msg.Dump();
            //msg2.Dump();
        }
    }

    void Test() {
        //TestEnv1::Test();
        //TestEnv2::Test();
    }
}
