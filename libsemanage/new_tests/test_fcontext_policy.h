/* source:
(typeattribute cil_gen_require)
(roleattribute cil_gen_require)
(handleunknown allow)
(mls true)
(policycap network_peer_controls)
(policycap open_perms)
(sid security)
(sidorder (security))
(sensitivity s0)
(sensitivityorder (s0))
(user system_u)
(userrole system_u object_r)
(userlevel system_u (s0))
(userrange system_u ((s0) (s0)))
(role object_r)
(roletype object_r first_t)
(roletype object_r second_t)
(roletype object_r third_t)
(type first_t)
(type second_t)
(type third_t)
(sidcontext security (system_u object_r first_t ((s0) (s0))))
(class test_class (test_perm))
(classorder (test_class))
(allow first_t self (test_class (test_perm)))
*/

char FCONTEXTS[] =
    "/etc/selinux(/.*) -s system_u:object_r:first_t:s0\n"
    "/etc/selinux/targeted -- system_u:object_r:second_t:s0\n"
    "/etc/selinux(/.*) -b system_u:object_r:third_t:s0\n";
unsigned int FCONTEXTS_LEN = sizeof FCONTEXTS;

#define FCONTEXTS_COUNT 3

#define FCONTEXT1_EXPR "/etc/selinux(/.*)"
#define FCONTEXT1_TYPE SEMANAGE_FCONTEXT_SOCK
#define FCONTEXT1_CON "system_u:object_r:first_t:s0"

#define FCONTEXT2_EXPR "/etc/selinux/targeted"
#define FCONTEXT2_TYPE SEMANAGE_FCONTEXT_REG
#define FCONTEXT2_CON "system_u:object_r:second_t:s0"

#define FCONTEXT3_EXPR "/etc/selinux(/.*)"
#define FCONTEXT3_TYPE SEMANAGE_FCONTEXT_BLOCK
#define FCONTEXT3_CON "system_u:object_r:third_t:s0"

#define FCONTEXT_NONEXISTENT_EXPR "/asdf"
#define FCONTEXT_NONEXISTENT_TYPE SEMANAGE_FCONTEXT_ALL

unsigned char FCONTEXT_POLICY[] = {
  0x8c, 0xff, 0x7c, 0xf9, 0x08, 0x00, 0x00, 0x00, 0x53, 0x45, 0x20, 0x4c,
  0x69, 0x6e, 0x75, 0x78, 0x1f, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
  0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x74, 0x65, 0x73, 0x74, 0x5f, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x09, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x70,
  0x65, 0x72, 0x6d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6f, 0x62, 0x6a, 0x65, 0x63,
  0x74, 0x5f, 0x72, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x5f, 0x74, 0x07,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x66, 0x69, 0x72, 0x73, 0x74, 0x5f, 0x74, 0x07, 0x00,
  0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x74, 0x68, 0x69, 0x72, 0x64, 0x5f, 0x74, 0x01, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x5f,
  0x75, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x73, 0x30, 0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
  0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned int FCONTEXT_POLICY_LEN = 595;
