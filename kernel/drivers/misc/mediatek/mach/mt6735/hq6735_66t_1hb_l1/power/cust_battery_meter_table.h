#ifndef _CUST_BATTERY_METER_TABLE_H
#define _CUST_BATTERY_METER_TABLE_H

#include <mach/mt_typedefs.h>

// ============================================================
// define
// ============================================================
#define BAT_NTC_10 1
#define BAT_NTC_47 0

#if (BAT_NTC_10 == 1)
#define RBAT_PULL_UP_R             16900	
#endif

#if (BAT_NTC_47 == 1)
#define RBAT_PULL_UP_R             61900	
#endif

#define RBAT_PULL_UP_VOLT          1800



// ============================================================
// ENUM
// ============================================================

// ============================================================
// structure
// ============================================================

// ============================================================
// typedef
// ============================================================

char* g_battery_manufacturer[]=
{
	"SUNWODA",  	//欣旺达
	"SCUD",  	//飞毛腿
	"unknow"
};

typedef struct _BATTERY_PROFILE_STRUC
{
    kal_int32 percentage;
    kal_int32 voltage;
} BATTERY_PROFILE_STRUC, *BATTERY_PROFILE_STRUC_P;

typedef struct _R_PROFILE_STRUC
{
    kal_int32 resistance; // Ohm
    kal_int32 voltage;
} R_PROFILE_STRUC, *R_PROFILE_STRUC_P;

typedef enum
{
    T1_0C,
    T2_25C,
    T3_50C
} PROFILE_TEMPERATURE;

// ============================================================
// External Variables
// ============================================================

// ============================================================
// External function
// ============================================================

// ============================================================
// <DOD, Battery_Voltage> Table
// ============================================================
#if (BAT_NTC_10 == 1)
    BATT_TEMPERATURE Batt_Temperature_Table[2][17] = {
       { {-20,70377},
        {-15,55070},
        {-10,43446},
        { -5,34532},
        {  0,27640},
        {  5,22271},
        { 10,18066},
        { 15,14744},
        { 20,12111},
        { 25,10000},
        { 30,8309},
        { 35,6941},
        { 40,5828},
        { 45,4916},
        { 50,4165},
        { 55,3543},
        { 60,2527},
       },
       { {-20,70377},
        {-15,55070},
        {-10,43446},
        { -5,34532},
        {  0,27640},
        {  5,22271},
        { 10,18066},
        { 15,14744},
        { 20,12111},
        { 25,10000},
        { 30,8309},
        { 35,6941},
        { 40,5828},
        { 45,4916},
        { 50,4165},
        { 55,3543},
        { 60,2527},
       },
    };
#endif

#if (BAT_NTC_47 == 1)
    BATT_TEMPERATURE Batt_Temperature_Table[2][17] = {
       {{-20,483954},
        {-15,360850},
        {-10,271697},
        { -5,206463},
        {  0,158214},
        {  5,122259},
        { 10,95227},
        { 15,74730},
        { 20,59065},
        { 25,47000},
        { 30,37643},
        { 35,30334},
        { 40,24591},
        { 45,20048},
        { 50,16433},
        { 55,13539},
        { 60,11210},
	   },
	   {{-20,483954},
        {-15,360850},
        {-10,271697},
        { -5,206463},
        {  0,158214},
        {  5,122259},
        { 10,95227},
        { 15,74730},
        { 20,59065},
        { 25,47000},
        { 30,37643},
        { 35,30334},
        { 40,24591},
        { 45,20048},
        { 50,16433},
        { 55,13539},
        { 60,11210},
	   },
    };
#endif

// T0 -10C
BATTERY_PROFILE_STRUC battery_profile_t0[2][76] =
{
   {    {0   , 4332},         
	{1   , 4296},         
	{3   , 4268},         
	{4   , 4244},         
	{6   , 4223},         
	{7   , 4203},         
	{8   , 4185},         
	{10  , 4167},         
	{11  , 4150},         
	{13  , 4133},         
	{14  , 4116},         
	{16  , 4101},         
	{17  , 4089},         
	{18  , 4074},         
	{20  , 4056},         
	{21  , 4035},         
	{23  , 4014},         
	{24  , 3992},         
	{25  , 3975},         
	{27  , 3960},         
	{28  , 3948},         
	{30  , 3937},         
	{31  , 3928},         
	{33  , 3918},         
	{34  , 3909},         
	{35  , 3900},         
	{37  , 3890},         
	{38  , 3882},         
	{40  , 3873},         
	{41  , 3864},         
	{42  , 3856},         
	{44  , 3847},         
	{45  , 3839},         
	{47  , 3831},         
	{48  , 3824},         
	{50  , 3816},         
	{51  , 3809},         
	{52  , 3801},         
	{54  , 3796},         
	{55  , 3789},         
	{57  , 3784},         
	{58  , 3779},         
	{59  , 3774},         
	{61  , 3769},        
	{62  , 3766},         
	{64  , 3761},         
	{65  , 3757},         
	{66  , 3754},         
	{68  , 3751},         
	{69  , 3748},        
	{71  , 3743},         
	{72  , 3740},        
	{74  , 3737},     
	{75  , 3733},         
	{76  , 3729},         
	{78  , 3725},         
	{79  , 3720},         
	{81  , 3714},         
	{82  , 3709},         
	{83  , 3703},         
	{85  , 3697},          
  {86 , 3690},
  {88 , 3682},
  {89 , 3674},
  {91 , 3665},
  {92 , 3654},
  {93 , 3641},
  {95 , 3626},
  {96 , 3607},
  {98 , 3582},
	{99  ,3548}, 
	{100 , 3521},
	{100 , 3521},
	{100 , 3521},
	{100 , 3521},
	{100 , 3521}	
	},
	{{  0   ,  4332 },
{  1   ,  4296 },
{  3   ,  4276 },
{  4   ,  4259 },
{  5   ,  4242 },
{  7   ,  4226 },
{  8   ,  4209 },
{  10  ,  4195 },
{  11  ,  4179 },
{  12  ,  4164 },
{  14  ,  4149 },
{  15  ,  4134 },
{  16  ,  4119 },
{  18  ,  4105 },
{  19  ,  4092 },
{  20  ,  4080 },
{  22  ,  4066 },
{  23  ,  4049 },
{  25  ,  4028 },
{  26  ,  4009 },
{  27  ,  3991 },
{  29  ,  3976 },
{  30  ,  3963 },
{  31  ,  3951 },
{  33  ,  3942 },
{  34  ,  3932 },
{  35  ,  3924 },
{  37  ,  3914 },
{  38  ,  3904 },
{  40  ,  3894 },
{  41  ,  3885 },
{  42  ,  3876 },
{  44  ,  3867 },
{  45  ,  3860 },
{  46  ,  3852 },
{  48  ,  3844 },
{  49  ,  3837 },
{  50  ,  3831 },
{  52  ,  3824 },
{  53  ,  3819 },
{  55  ,  3813 },
{  56  ,  3807 },
{  57  ,  3803 },
{  59  ,  3798 },
{  60  ,  3795 },
{  61  ,  3792 },
{  63  ,  3789 },
{  64  ,  3787 },
{  65  ,  3785 },
{  67  ,  3782 },
{  68  ,  3779 },
{  70  ,  3777 },
{  71  ,  3774 },
{  72  ,  3771 },
{  74  ,  3768 },
{  75  ,  3764 },
{  76  ,  3760 },
{  78  ,  3756 },
{  79  ,  3750 },
{  80  ,  3744 },
{  82  ,  3739 },
{  83  ,  3732 },
{  85  ,  3725 },
{  86  ,  3718 },
{  87  ,  3711 },
{  89  ,  3705 },
{  90  ,  3697 },
{  91  ,  3689 },
{  93  ,  3678 },
{  94  ,  3662 },
{  95  ,  3631 },
{  97  ,  3600 },
{  98  ,  3550 },
{  99  ,  3420 },
{  100 ,  3400 },
{  100 ,  3400 } 
}
};        
// T1 0C 
BATTERY_PROFILE_STRUC battery_profile_t1[2][76] =
{
   {{0   ,4342},         
	{1   ,4314},         
	{3   ,4293},         
	{4   ,4275},             
	{5   ,4258},         
	{7   ,4241},         
	{8   ,4226},         
	{10  ,4211},         
	{11  ,4196},         
	{12  ,4182},         
	{14  ,4168},         
	{15  ,4154},         
	{16  ,4139},         
	{18 ,4125},         
	{19  ,4111},         
	{20  ,4097},         
	{22 ,4087},         
	{23  ,4075},         
	{24  ,4061},         
	{26  ,4042},         
	{27  ,4019},         
	{29  ,3996},         
	{30  ,3978},         
	{31 ,3963},         
	{33  ,3951},         
	{34  ,3941},         
	{35  ,3931},         
	{37 ,3922},         
	{38  ,3914},         
	{39  ,3905},         
	{41  ,3897},         
	{42  ,3888},         
	{44  ,3880},         
	{45  ,3872},         
	{46  ,3864},         
	{48  ,3856},         
	{49  ,3849},         
	{50  ,3842},         
	{52  ,3836},         
	{53  ,3829},         
	{54  ,3823},         
	{56  ,3816},         
	{57  ,3811},         
	{58  ,3805},         
	{60  ,3799},         
	{61  ,3795},         
	{63  ,3790},         
	{64  ,3785},         
	{65  ,3782},         
	{67  ,3778},         
	{68  ,3775},         
	{69  ,3770},         
	{71  ,3767},         
	{72  ,3764},         
	{73  ,3760},         
	{75  ,3756},         
	{76  ,3752},         
	{77  ,3748},         
        {79  ,3743},
	{80  ,3738},         
	{82  ,3732},          
  {83  ,3726},
  {84  ,3721},
  {86 ,3717},
  {87  ,3712},
  {88 ,3708},
  {90 ,3704},
  {91 ,3699},
  {92 ,3692},
  {94 ,3682},
	{95 ,3663}, 
	{97 ,3631},
	{98 ,3579},
	{99 ,3492},
	{100 ,3331},
	{100 ,3331}
	},
	{{ 0    , 4331  },
{ 1    , 4307  },
{ 3    , 4288  },
{ 4    , 4271  },
{ 5    , 4254  },
{ 7    , 4238  },
{ 8    , 4222  },
{ 10   , 4207  },
{ 11   , 4192  },
{ 12   , 4177  },
{ 14   , 4162  },
{ 15   , 4148  },
{ 16   , 4134  },
{ 18   , 4120  },
{ 19   , 4105  },
{ 20   , 4094  },
{ 22   , 4084  },
{ 23   , 4071  },
{ 25   , 4054  },
{ 26   , 4036  },
{ 27   , 4017  },
{ 29   , 4001  },
{ 30   , 3987  },
{ 31   , 3973  },
{ 33   , 3963  },
{ 34   , 3953  },
{ 35   , 3943  },
{ 37   , 3932  },
{ 38   , 3921  },
{ 40   , 3909  },
{ 41   , 3899  },
{ 42   , 3888  },
{ 44   , 3878  },
{ 45   , 3869  },
{ 46   , 3861  },
{ 48   , 3853  },
{ 49   , 3845  },
{ 50   , 3839  },
{ 52   , 3832  },
{ 53   , 3826  },
{ 55   , 3821  },
{ 56   , 3815  },
{ 57   , 3811  },
{ 59   , 3806  },
{ 60   , 3801  },
{ 61   , 3797  },
{ 63   , 3793  },
{ 64   , 3790  },
{ 65   , 3788  },
{ 67   , 3786  },
{ 68   , 3784  },
{ 70   , 3782  },
{ 71   , 3780  },
{ 72   , 3778  },
{ 74   , 3775  },
{ 75   , 3772  },
{ 76   , 3769  },
{ 78   , 3766  },
{ 79   , 3761  },
{ 80   , 3755  },
{ 82   , 3749  },
{ 83   , 3743  },
{ 85   , 3735  },
{ 86   , 3726  },
{ 87   , 3718  },
{ 89   , 3708  },
{ 90   , 3700  },
{ 91   , 3694  },
{ 93   , 3688  },
{ 94   , 3679  },
{ 95   , 3666  },
{ 97   , 3638  },
{ 98   , 3579  },
{ 100  , 3400  },
{ 100  , 3400  },
{ 100  , 3400  } }
};    
// T2 25C
BATTERY_PROFILE_STRUC battery_profile_t2[2][76] =
{
   {{0   , 4335},         
	{1   , 4317},         
	{3   , 4301},         
	{4   , 4286},         
	{5   , 4270},         
	{7   , 4255},         
	{8   , 4240},         
	{10  , 4226},         
	{11  , 4211},         
	{12  , 4196},         
	{14  , 4182},         
	{15  , 4167},         
	{16  , 4154},         
	{18  , 4139},         
	{19  , 4125},         
	{20  , 4112},         
	{22  , 4098},         
	{23  , 4085},         
	{25  , 4073},         
	{26  , 4066},         
	{27  , 4055},         
	{29  , 4035},         
	{30  , 4014},         
	{31  , 3997},         
	{33  , 3985},         
	{34  , 3977},         
	{35  , 3971},         
	{37  , 3964},         
	{38  , 3953},         
	{39  , 3941},         
	{41  , 3927},         
	{42  , 3913},         
	{44  , 3898},         
	{45  , 3885},         
	{46  , 3875},         
	{48  , 3865},         
	{49  , 3857},         
	{50  , 3849},         
	{52  , 3842},         
	{53  , 3836},         
	{54  , 3831},         
	{56  , 3824},         
	{57  , 3819},         
	{59  , 3813},         
	{60  , 3808},         
	{61  , 3804},         
	{63  , 3800},         
	{64  , 3795},         
	{65  , 3792},         
	{67  , 3788},         
	{68  , 3783},         
	{69  , 3780},         
	{71  , 3776},         
	{72  , 3773},         
	{74  , 3769},         
	{75  , 3765},         
	{76  , 3762},         
	{78  , 3757},         
	{79  , 3752},         
	{80  , 3748},         
	{82  , 3743},          
  {83  , 3735},
  {84  , 3725},
  {86  , 3716},
  {87  , 3704},
  {88  , 3695},
  {90  , 3692},
  {91  , 3690},
  {93  , 3689},
  {94  , 3687},
	{95  , 3683}, 
	{97  , 3653},
	{98  , 3586},
	{99  , 3487},
	{100 , 3299},
	{100 , 3299}
	},
	{{  0   ,  4333 },
{  1   ,  4311 },
{  3   ,  4293 },
{  4   ,  4276 },
{  5   ,  4259 },
{  7   ,  4243 },
{  8   ,  4227 },
{  10  ,  4211 },
{  11  ,  4196 },
{  12  ,  4181 },
{  14  ,  4166 },
{  15  ,  4151 },
{  16  ,  4137 },
{  18  ,  4123 },
{  19  ,  4108 },
{  21  ,  4095 },
{  22  ,  4082 },
{  23  ,  4070 },
{  25  ,  4059 },
{  26  ,  4043 },
{  27  ,  4028 },
{  29  ,  4014 },
{  30  ,  4002 },
{  31  ,  3991 },
{  33  ,  3982 },
{  34  ,  3973 },
{  36  ,  3961 },
{  37  ,  3952 },
{  38  ,  3941 },
{  40  ,  3930 },
{  41  ,  3918 },
{  42  ,  3905 },
{  44  ,  3891 },
{  45  ,  3877 },
{  46  ,  3865 },
{  48  ,  3857 },
{  49  ,  3848 },
{  51  ,  3841 },
{  52  ,  3834 },
{  53  ,  3828 },
{  55  ,  3823 },
{  56  ,  3817 },
{  57  ,  3812 },
{  59  ,  3807 },
{  60  ,  3803 },
{  62  ,  3798 },
{  63  ,  3795 },
{  64  ,  3791 },
{  66  ,  3788 },
{  67  ,  3785 },
{  68  ,  3782 },
{  70  ,  3779 },
{  71  ,  3775 },
{  72  ,  3772 },
{  74  ,  3767 },
{  75  ,  3762 },
{  77  ,  3758 },
{  78  ,  3752 },
{  79  ,  3748 },
{  81  ,  3743 },
{  82  ,  3738 },
{  83  ,  3733 },
{  85  ,  3725 },
{  86  ,  3717 },
{  87  ,  3708 },
{  89  ,  3697 },
{  90  ,  3689 },
{  92  ,  3685 },
{  93  ,  3682 },
{  94  ,  3678 },
{  96  ,  3669 },
{  97  ,  3638 },
{  98  ,  3572 },
{  100 ,  3400 },
{  100 ,  3400 },
{  100 ,  3400 } 
}
};        

// T3 50C
BATTERY_PROFILE_STRUC battery_profile_t3[2][76] =
{
   {{0   , 4327},         
	{1   , 4312},         
	{3   , 4296},         
	{4   , 4280},         
	{6   , 4265},         
	{7   , 4250},         
	{8   , 4235},         
	{10  , 4220},         
	{11  , 4205},         
	{12  , 4190},         
	{14  , 4175},         
	{15  , 4161},         
	{17  , 4147},         
	{18  , 4133},         
	{19  , 4118},         
	{21  , 4105},         
	{22  , 4092},         
	{23  , 4079},         
	{25  , 4066},         
	{26  , 4053},         
	{28  , 4040},         
	{29  , 4028},         
	{30  , 4017},         
	{32  , 4005},         
	{33  , 3994},         
	{34  , 3984},         
	{36  , 3973},         
	{37  , 3963},         
	{39  , 3953},         
	{40  , 3942},         
	{41  , 3930},         
	{43  , 3912},         
	{44  , 3896},         
	{45  , 3883},         
	{47  , 3873},         
	{48  , 3865},         
	{50  , 3857},         
	{51  , 3850},         
	{52  , 3843},         
	{54  , 3837},         
	{55  , 3830},         
	{56  , 3824},         
	{58  , 3819},         
	{59  , 3814},         
	{61  , 3810},         
	{62  , 3805},         
	{63  , 3800},         
	{65  , 3796},         
	{66  , 3792},         
	{67  , 3788},         
	{69  , 3784},         
	{70  , 3779},         
	{72  , 3771},         
	{73  , 3762},         
	{74  , 3757},         
	{76  , 3752},         
	{77  , 3746},         
	{78  , 3739},         
	{80  , 3734},         
	{81  , 3729},         
	{83  , 3723},          
  {84  , 3715},
  {85  , 3705},
  {87  , 3695},
  {88  , 3684},
  {89  , 3681},
  {91  , 3679},
  {92  , 3679},
  {94  , 3677},
  {95  , 3672},
	{97  , 3647}, 
	{98  , 3587},
	{99  , 3498},
	{100 , 3353},
	{100 , 3353},
	{100 , 3353}
	},
	 {{ 0   , 4330  },
{ 1   , 4297  },
{ 3   , 4280  },
{ 4   , 4263  },
{ 6   , 4247  },
{ 7   , 4231  },
{ 8   , 4215  },
{ 10  , 4200  },
{ 11  , 4184  },
{ 12  , 4169  },
{ 14  , 4155  },
{ 15  , 4140  },
{ 17  , 4126  },
{ 18  , 4111  },
{ 19  , 4097  },
{ 21  , 4084  },
{ 22  , 4071  },
{ 24  , 4058  },
{ 25  , 4045  },
{ 26  , 4032  },
{ 28  , 4019  },
{ 29  , 4007  },
{ 30  , 3996  },
{ 32  , 3985  },
{ 33  , 3974  },
{ 35  , 3963  },
{ 36  , 3953  },
{ 37  , 3943  },
{ 39  , 3932  },
{ 40  , 3920  },
{ 41  , 3907  },
{ 43  , 3893  },
{ 44  , 3878  },
{ 46  , 3867  },
{ 47  , 3859  },
{ 48  , 3850  },
{ 50  , 3842  },
{ 51  , 3836  },
{ 53  , 3829  },
{ 54  , 3824  },
{ 55  , 3818  },
{ 57  , 3813  },
{ 58  , 3808  },
{ 59  , 3803  },
{ 61  , 3799  },
{ 62  , 3795  },
{ 64  , 3791  },
{ 65  , 3787  },
{ 66  , 3783  },
{ 68  , 3779  },
{ 69  , 3776  },
{ 71  , 3771  },
{ 72  , 3763  },
{ 73  , 3753  },
{ 75  , 3748  },
{ 76  , 3743  },
{ 77  , 3737  },
{ 79  , 3732  },
{ 80  , 3728  },
{ 82  , 3723  },
{ 83  , 3717  },
{ 84  , 3709  },
{ 86  , 3702  },
{ 87  , 3694  },
{ 89  , 3682  },
{ 90  , 3677  },
{ 91  , 3674  },
{ 93  , 3671  },
{ 94  , 3667  },
{ 95  , 3655  },
{ 97  , 3614  },
{ 98  , 3548  },
{ 100 , 3400  },
{ 100 , 3400  },
{ 100 , 3400  },
{ 100 , 3400  } 
}
};           

// battery profile for actual temperature. The size should be the same as T1, T2 and T3
BATTERY_PROFILE_STRUC battery_profile_temperature[2][76] =
{
   {{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 }, 
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 }
	},
	 {{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 }, 
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
        {0  , 0 },
        {0  , 0 } 
	}
};    

// ============================================================
// <Rbat, Battery_Voltage> Table
// ============================================================
// T0 -10C
R_PROFILE_STRUC r_profile_t0[2][76] =
{
   {    {2180  ,   4332},
{2180  ,   4296},
{2148  ,   4268},
{2105  ,   4244},
{2068  ,   4223},
{2030  ,   4203},
{2000  ,   4185},
{1968  ,   4167},
{1940  ,   4150},
{1915  ,   4133},
{1888  ,   4116},
{1865  ,   4101},
{1853  ,   4089},
{1835  ,   4074},
{1808  ,   4056},
{1780  ,   4035},
{1748  ,   4014},
{1713  ,   3992},
{1695  ,   3975},
{1680  ,   3960},
{1673  ,   3948},
{1668  ,   3937},
{1665  ,   3928},
{1660  ,   3918},
{1658  ,   3909},
{1658  ,   3900},
{1653  ,   3890},
{1650  ,   3882},
{1653  ,   3873},
{1645  ,   3864},
{1650  ,   3856},
{1648  ,   3847},
{1645  ,   3839},
{1645  ,   3831},
{1648  ,   3824},
{1645  ,   3816},
{1648  ,   3809},
{1648  ,   3801},
{1655  ,   3796},
{1658  ,   3789},
{1668  ,   3784},
{1683  ,   3779},
{1695  ,   3774},
{1708  ,   3769},
{1728  ,   3766},
{1743  ,   3761},
{1763  ,   3757},
{1785  ,   3754},
{1808  ,   3751},
{1835  ,   3748},
{1860  ,   3743},
{1888  ,   3740},
{1915  ,   3737},
{1945  ,   3733},
{1980  ,   3729},
{2005  ,   3725},
{2040  ,   3720},
{2068  ,   3714},
{2100  ,   3709},
{2130  ,   3703},
{2168  ,   3697},
{2203  ,   3690},
{2235  ,   3682},
{2270  ,   3674},
{2315  ,   3665},
{2348  ,   3654},
{2395  ,   3641},
{2440  ,   3626},
{2495  ,   3607},
{2555  ,   3582},
{2630  ,   3548},
{8803  ,   3521},
{8803  ,   3521},
{8803  ,   3521},
{8803  ,   3521},
{8803  ,   3521}},
{{  1349 ,  4332 },
{  1253 ,  4296 },
{  1243 ,  4276 },
{  1228 ,  4259 },
{  1214 ,  4242 },
{  1195 ,  4226 },
{  1188 ,  4209 },
{  1170 ,  4195 },
{  1161 ,  4179 },
{  1139 ,  4164 },
{  1141 ,  4149 },
{  1130 ,  4134 },
{  1132 ,  4119 },
{  1130 ,  4105 },
{  1126 ,  4092 },
{  1113 ,  4080 },
{  1110 ,  4066 },
{  1094 ,  4049 },
{  1077 ,  4028 },
{  1060 ,  4009 },
{  1048 ,  3991 },
{  1042 ,  3976 },
{  1032 ,  3963 },
{  1029 ,  3951 },
{  1029 ,  3942 },
{  1028 ,  3932 },
{  1028 ,  3924 },
{  1028 ,  3914 },
{  1023 ,  3904 },
{  1025 ,  3894 },
{  1028 ,  3885 },
{  1029 ,  3876 },
{  1031 ,  3867 },
{  1034 ,  3860 },
{  1039 ,  3852 },
{  1042 ,  3844 },
{  1048 ,  3837 },
{  1054 ,  3831 },
{  1061 ,  3824 },
{  1065 ,  3819 },
{  1068 ,  3813 },
{  1076 ,  3807 },
{  1087 ,  3803 },
{  1093 ,  3798 },
{  1104 ,  3795 },
{  1119 ,  3792 },
{  1136 ,  3789 },
{  1155 ,  3787 },
{  1178 ,  3785 },
{  1194 ,  3782 },
{  1220 ,  3779 },
{  1246 ,  3777 },
{  1276 ,  3774 },
{  1304 ,  3771 },
{  1341 ,  3768 },
{  1377 ,  3764 },
{  1419 ,  3760 },
{  1459 ,  3756 },
{  1501 ,  3750 },
{  1547 ,  3744 },
{  1598 ,  3739 },
{  1645 ,  3732 },
{  1699 ,  3725 },
{  1759 ,  3718 },
{  1818 ,  3711 },
{  1887 ,  3705 },
{  1969 ,  3697 },
{  2063 ,  3689 },
{  2175 ,  3678 },
{  2330 ,  3662 },
{  2535 ,  3631 },
{  2535 ,  3600 },
{  2535 ,  3550 },
{  2535 ,  3420 },
{  2535 ,  3400 }, 
{  2535 ,  3400 }}
};      

// T1 0C
R_PROFILE_STRUC r_profile_t1[2][76] =
{
   { {1065  ,4342 }, 
     {1065  ,4314 }, 
     {1068  ,4293 }, 
     {1068  ,4275 }, 
     {1060  ,4258 }, 
     {1058  ,4241 }, 
     {1053  ,4226 }, 
     {1048  ,4211 }, 
     {1045  ,4196 }, 
     {1043  ,4182 }, 
     {1040  ,4168 }, 
     {1033  ,4154 }, 
     {1020  ,4139 }, 
     {1020  ,4125 }, 
     {1015  ,4111 }, 
     {1005  ,4097 }, 
     {1008  ,4087 }, 
     {1010  ,4075 }, 
     {1003  ,4061 }, 
     {980   ,4042 }, 
     {953   ,4019 }, 
     {928   ,3996 }, 
     {910   ,3978 }, 
     {905   ,3963 }, 
     {898   ,3951 }, 
     {900   ,3941 }, 
     {898   ,3931 }, 
     {895   ,3922 }, 
     {898   ,3914 }, 
     {893   ,3905 }, 
     {888   ,3897 }, 
     {885   ,3888 }, 
     {888   ,3880 }, 
     {888   ,3872 }, 
     {888   ,3864 }, 
     {888   ,3856 }, 
     {890   ,3849 }, 
     {893   ,3842 }, 
     {895   ,3836 }, 
     {898   ,3829 }, 
     {900   ,3823 }, 
     {903   ,3816 }, 
     {908   ,3811 }, 
     {910   ,3805 }, 
     {913   ,3799 }, 
     {923   ,3795 }, 
     {928   ,3790 }, 
     {933   ,3785 }, 
     {950   ,3782 }, 
     {963   ,3778 }, 
     {980   ,3775 }, 
     {995   ,3770 }, 
     {1015  ,3767 }, 
     {1035  ,3764 }, 
     {1055  ,3760 }, 
     {1075  ,3756 }, 
     {1098  ,3752 }, 
     {1120  ,3748 }, 
     {1143  ,3743 }, 
     {1170  ,3738 }, 
     {1190  ,3732 }, 
     {1215  ,3726 }, 
     {1243  ,3721 }, 
     {1278  ,3717 }, 
     {1318  ,3712 }, 
     {1365  ,3708 }, 
     {1418  ,3704 }, 
     {1480  ,3699 }, 
     {1548  ,3692 }, 
     {1620  ,3682 }, 
     {1685  ,3663 }, 
     {1753  ,3631 }, 
     {1823  ,3579 }, 
     {1910  ,3492 }, 
     {1910  ,3331 }, 
    {1910  ,3331 }
	},
	{ {  604  ,  4331 },
{  647  ,  4307 },
{  645  ,  4288 },
{  651  ,  4271 },
{  645  ,  4254 },
{  649  ,  4238 },
{  645  ,  4222 },
{  646  ,  4207 },
{  643  ,  4192 },
{  642  ,  4177 },
{  645  ,  4162 },
{  645  ,  4148 },
{  644  ,  4134 },
{  642  ,  4120 },
{  643  ,  4105 },
{  642  ,  4094 },
{  653  ,  4084 },
{  653  ,  4071 },
{  649  ,  4054 },
{  645  ,  4036 },
{  642  ,  4017 },
{  636  ,  4001 },
{  633  ,  3987 },
{  623  ,  3973 },
{  623  ,  3963 },
{  617  ,  3953 },
{  619  ,  3943 },
{  612  ,  3932 },
{  606  ,  3921 },
{  605  ,  3909 },
{  606  ,  3899 },
{  597  ,  3888 },
{  598  ,  3878 },
{  602  ,  3869 },
{  600  ,  3861 },
{  603  ,  3853 },
{  603  ,  3845 },
{  608  ,  3839 },
{  612  ,  3832 },
{  614  ,  3826 },
{  617  ,  3821 },
{  623  ,  3815 },
{  629  ,  3811 },
{  633  ,  3806 },
{  639  ,  3801 },
{  639  ,  3797 },
{  645  ,  3793 },
{  654  ,  3790 },
{  656  ,  3788 },
{  665  ,  3786 },
{  681  ,  3784 },
{  685  ,  3782 },
{  698  ,  3780 },
{  710  ,  3778 },
{  713  ,  3775 },
{  746  ,  3772 },
{  760  ,  3769 },
{  777  ,  3766 },
{  795  ,  3761 },
{  819  ,  3755 },
{  836  ,  3749 },
{  862  ,  3743 },
{  896  ,  3735 },
{  918  ,  3726 },
{  958  ,  3718 },
{  1011 ,  3708 },
{  1076 ,  3700 },
{  1146 ,  3694 },
{  1273 ,  3688 },
{  1422 ,  3679 },
{  1597 ,  3666 },
{  1789 ,  3638 },
{  2015 ,  3579 },
{  2201 ,  3400 },
{  2201 ,  3400 },
{  2201 ,  3400 }}
};     

// T2 25C
R_PROFILE_STRUC r_profile_t2[2][76] =
{
   {   {248   ,4335 }, 
       {248   ,4317 }, 
       {253   ,4301 }, 
       {258   ,4286 }, 
       {253   ,4270 }, 
       {255   ,4255 }, 
       {253   ,4240 }, 
       {265   ,4226 }, 
       {258   ,4211 }, 
       {248   ,4196 }, 
       {250   ,4182 }, 
       {248   ,4167 }, 
       {258   ,4154 }, 
       {250   ,4139 }, 
       {263   ,4125 }, 
       {255   ,4112 }, 
       {260   ,4098 }, 
       {255   ,4085 }, 
       {260   ,4073 }, 
       {278   ,4066 }, 
       {283   ,4055 }, 
       {270   ,4035 }, 
       {270   ,4014 }, 
       {268   ,3997 }, 
       {263   ,3985 }, 
       {273   ,3977 }, 
       {275   ,3971 }, 
       {275   ,3964 }, 
       {270   ,3953 }, 
       {265   ,3941 }, 
       {253   ,3927 }, 
       {243   ,3913 }, 
       {225   ,3898 }, 
       {218   ,3885 }, 
       {218   ,3875 }, 
       {213   ,3865 }, 
       {210   ,3857 }, 
       {213   ,3849 }, 
       {215   ,3842 }, 
       {210   ,3836 }, 
       {213   ,3831 }, 
       {215   ,3824 }, 
       {213   ,3819 }, 
       {220   ,3813 }, 
       {225   ,3808 }, 
       {223   ,3804 }, 
       {220   ,3800 }, 
       {225   ,3795 }, 
       {228   ,3792 }, 
       {228   ,3788 }, 
       {225   ,3783 }, 
       {233   ,3780 }, 
       {230   ,3776 }, 
       {235   ,3773 }, 
       {235   ,3769 }, 
       {225   ,3765 }, 
       {233   ,3762 }, 
       {235   ,3757 }, 
       {233   ,3752 }, 
       {238   ,3748 }, 
       {240   ,3743 }, 
       {238   ,3735 }, 
       {243   ,3725 }, 
       {240   ,3716 }, 
       {238   ,3704 }, 
       {243   ,3695 }, 
       {258   ,3692 }, 
       {258   ,3690 }, 
       {275   ,3689 }, 
       {298   ,3687 }, 
       {313   ,3683 }, 
       {318   ,3653 }, 
       {323   ,3586 }, 
       {370   ,3484 }, 
       {453   ,3299 }, 
       {453  ,3299 }     
	},
	{{  240 , 4333  },
{  199 , 4311  },
{  143 , 4293  },
{  144 , 4276  },
{  146 , 4259  },
{  146 , 4243  },
{  147 , 4227  },
{  149 , 4211  },
{  147 , 4196  },
{  149 , 4181  },
{  147 , 4166  },
{  150 , 4151  },
{  150 , 4137  },
{  153 , 4123  },
{  155 , 4108  },
{  157 , 4095  },
{  158 , 4082  },
{  161 , 4070  },
{  163 , 4059  },
{  167 , 4043  },
{  167 , 4028  },
{  171 , 4014  },
{  169 , 4002  },
{  164 , 3991  },
{  163 , 3982  },
{  164 , 3973  },
{  169 , 3961  },
{  169 , 3952  },
{  172 , 3941  },
{  171 , 3930  },
{  169 , 3918  },
{  168 , 3905  },
{  160 , 3891  },
{  150 , 3877  },
{  141 , 3865  },
{  133 , 3857  },
{  133 , 3848  },
{  132 , 3841  },
{  132 , 3834  },
{  130 , 3828  },
{  130 , 3823  },
{  132 , 3817  },
{  133 , 3812  },
{  132 , 3807  },
{  133 , 3803  },
{  137 , 3798  },
{  135 , 3795  },
{  138 , 3791  },
{  138 , 3788  },
{  141 , 3785  },
{  143 , 3782  },
{  144 , 3779  },
{  144 , 3775  },
{  144 , 3772  },
{  146 , 3767  },
{  143 , 3762  },
{  140 , 3758  },
{  140 , 3752  },
{  140 , 3748  },
{  140 , 3743  },
{  143 , 3738  },
{  144 , 3733  },
{  152 , 3725  },
{  158 , 3717  },
{  166 , 3708  },
{  169 , 3697  },
{  164 , 3689  },
{  163 , 3685  },
{  166 , 3682  },
{  175 , 3678  },
{  194 , 3669  },
{  215 , 3638  },
{  226 , 3572  },
{  242 , 3400  },
{  242 , 3400  },
{  242 , 3400  }}
}; 

// T3 50C
R_PROFILE_STRUC r_profile_t3[2][76] =
{
   { {98    ,4327 }, 
    {98    ,4312 }, 
    {98    ,4296 }, 
    {98    ,4280 }, 
    {100   ,4265 }, 
    {103   ,4250 }, 
    {103   ,4235 }, 
    {105   ,4220 }, 
    {103   ,4205 }, 
    {103   ,4190 }, 
    {103   ,4175 }, 
    {105   ,4161 }, 
    {105   ,4147 }, 
    {108   ,4133 }, 
    {105   ,4118 }, 
    {108   ,4105 }, 
    {110   ,4092 }, 
    {113   ,4079 }, 
    {115   ,4066 }, 
    {115   ,4053 }, 
    {115   ,4040 }, 
    {118   ,4028 }, 
    {123   ,4017 }, 
    {123   ,4005 }, 
    {125   ,3994 }, 
    {130   ,3984 }, 
    {130   ,3973 }, 
    {135   ,3963 }, 
    {138   ,3953 }, 
    {140   ,3942 }, 
    {140   ,3930 }, 
    {123   ,3912 }, 
    {110   ,3896 }, 
    {103   ,3883 }, 
    {98    ,3873 }, 
    {100   ,3865 }, 
    {98    ,3857 }, 
    {100   ,3850 }, 
    {100   ,3843 }, 
    {103   ,3837 }, 
    {100   ,3830 }, 
    {103   ,3824 }, 
    {105   ,3819 }, 
    {108   ,3814 }, 
    {110   ,3810 }, 
    {110   ,3805 }, 
    {113   ,3800 }, 
    {113   ,3796 }, 
    {115   ,3792 }, 
    {115   ,3788 }, 
    {118   ,3784 }, 
    {115   ,3779 }, 
    {108   ,3771 }, 
    {103   ,3762 }, 
    {105   ,3757 }, 
    {108   ,3752 }, 
    {108   ,3746 }, 
    {103   ,3739 }, 
    {105   ,3734 }, 
    {105   ,3729 }, 
    {108   ,3723 }, 
    {108   ,3715 }, 
    {105   ,3705 }, 
    {105   ,3695 }, 
    {103   ,3684 }, 
    {103   ,3681 }, 
    {105   ,3679 }, 
    {113   ,3679 }, 
    {118   ,3677 }, 
    {118   ,3672 }, 
    {113   ,3647 }, 
    {118   ,3587 }, 
    {128   ,3498 }, 
    {163   ,3353 }, 
    {163  ,3353 }, 
    {163  ,3353 }	
    },
   {{  123 , 4330  },
{  85  , 4297  },
{  85  , 4280  },
{  85  , 4263  },
{  85  , 4247  },
{  85  , 4231  },
{  87  , 4215  },
{  87  , 4200  },
{  87  , 4184  },
{  87  , 4169  },
{  90  , 4155  },
{  89  , 4140  },
{  90  , 4126  },
{  90  , 4111  },
{  91  , 4097  },
{  93  , 4084  },
{  93  , 4071  },
{  96  , 4058  },
{  96  , 4045  },
{  96  , 4032  },
{  98  , 4019  },
{  99  , 4007  },
{  101 , 3996  },
{  102 , 3985  },
{  106 , 3974  },
{  107 , 3963  },
{  110 , 3953  },
{  112 , 3943  },
{  114 , 3932  },
{  112 , 3920  },
{  109 , 3907  },
{  103 , 3893  },
{  93  , 3878  },
{  88  , 3867  },
{  88  , 3859  },
{  87  , 3850  },
{  87  , 3842  },
{  88  , 3836  },
{  87  , 3829  },
{  90  , 3824  },
{  88  , 3818  },
{  90  , 3813  },
{  91  , 3808  },
{  92  , 3803  },
{  95  , 3799  },
{  94  , 3795  },
{  98  , 3791  },
{  98  , 3787  },
{  99  , 3783  },
{  99  , 3779  },
{  101 , 3776  },
{  99  , 3771  },
{  91  , 3763  },
{  85  , 3753  },
{  89  , 3748  },
{  88  , 3743  },
{  88  , 3737  },
{  89  , 3732  },
{  90  , 3728  },
{  93  , 3723  },
{  91  , 3717  },
{  91  , 3709  },
{  91  , 3702  },
{  96  , 3694  },
{  92  , 3682  },
{  91  , 3677  },
{  95  , 3674  },
{  99  , 3671  },
{  106 , 3667  },
{  112 , 3655  },
{  110 , 3614  },
{  119 , 3548  },
{  128 , 3400  },
{  128 , 3400  },
{  128 , 3400  },
{  128 , 3400  }}
}; 

// r-table profile for actual temperature. The size should be the same as T1, T2 and T3
R_PROFILE_STRUC r_profile_temperature[2][76] =
{
   {{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 }, 
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 }
   },
   {{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 }, 
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },  
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
        {0  , 0 }}
};    

// ============================================================
// function prototype
// ============================================================
int fgauge_get_saddles(void);
BATTERY_PROFILE_STRUC_P fgauge_get_profile(kal_uint32 temperature);

int fgauge_get_saddles_r_table(void);
R_PROFILE_STRUC_P fgauge_get_profile_r_table(kal_uint32 temperature);

#endif	//#ifndef _CUST_BATTERY_METER_TABLE_H

