![image-20250407162052810](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504071620843.png)

```c
//正点原子的翻译
#if 1 /* 设置为1，以启用内容 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*********************************************************************************

                                        颜色设置
                                        
 ***********************************************************************************/

/* 颜色深度:1(1字节每像素)，8(RGB332)， 16(RGB565)， 32(ARGB8888) */
#define LV_COLOR_DEPTH 16

/* 交换2个字节的RGB565颜色，在显示接口是8位的时候有用(例如SPI) */
#define LV_COLOR_16_SWAP 0

/* 1：启用屏幕透明度。
 * 这对于OSD菜单、视频播放器和重叠的GUI有用。
 * 需要32位深度的颜色，因为阿尔法表示透明度，并且屏幕的' bg_opa '应该设置为非LV_OPA_COVER值 */
#define LV_COLOR_SCREEN_TRANSP 0

/* 调整颜色混合功能四舍五入。gpu可能会以不同的方式计算颜色混合。
 * 0：向下舍入，64：从 x.75向上舍入，128：从一半向上舍入，192：从x.25 向上舍入，254：向上舍入 */
#define LV_COLOR_MIX_ROUND_OFS (LV_COLOR_DEPTH == 32 ? 0: 128)

/* 如果使用色度键，将不会绘制这种颜色的图像像素，类似于抠掉绿幕背景的效果 */
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)         /* 纯绿色 */



/*********************************************************************************

                                        内存设置
                                        
 ***********************************************************************************/

#define LV_MEM_CUSTOM           0                           /* 0：使用LVGL自带的内存管理，1：使用自研内存管理 */

#if LV_MEM_CUSTOM == 0                                      /* 使用LVGL自带内存管理 */

    #define LV_MEM_SIZE        (48U * 1024U)                /* 所管理的空间大小由这里分配，单位：字节 */

    #define LV_MEM_ADR          0                           /* 0：使用大数组创建内存池；其他：使用地址段来创建内存池 */

    #if LV_MEM_ADR == 0                                     /* 使用内存分配函数来创建LVGL的内存池 */
//        #define LV_MEM_POOL_INCLUDE     your_alloc_library  /* 头文件路径 */
//        #define LV_MEM_POOL_ALLOC       your_alloc          /* 内存分配函数 */
    #endif
#else                                                       /* 使用自研内存管理 */

    #define LV_MEM_CUSTOM_INCLUDE       <stdlib.h>          /* 头文件路径 */
    #define LV_MEM_CUSTOM_ALLOC         malloc              /* 内存分配函数 */
    #define LV_MEM_CUSTOM_FREE          free                /* 内存释放函数 */
    #define LV_MEM_CUSTOM_REALLOC       realloc             /* 内存重新分配函数 */

#endif 

/* 在渲染和其他内部处理机制期间使用的中间内存缓冲区的数量。
 * 如果没有足够的缓冲区，你会看到一个错误日志信息 */
#define LV_MEM_BUF_MAX_NUM                  16

/* 使用标准的 `memcpy` 和 `memset` 代替LVGL自己的函数 */
#define LV_MEMCPY_MEMSET_STD                0



/*********************************************************************************

                                        HAL 设置
                                        
 ***********************************************************************************/
 
/* 默认的显示刷新周期。LVGL按这个周期重绘修改过的区域 一般5~30都可以*/
#define LV_DISP_DEF_REFR_PERIOD             5      /*[ms]*/

/* 输入设备的读取周期(以毫秒为单位) 5~30*/
#define LV_INDEV_DEF_READ_PERIOD            5     /*[ms]*/

/* 0：手动调用 `lv_tick_inc()`以提供时钟源，1：使用自定义时钟源 */
#define LV_TICK_CUSTOM                      1

#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE          "FreeRTOS.h"                /* 系统时间函数头 */
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR    (xTaskGetTickCount())       /* 计算系统当前时间的表达式(以毫秒为单位) */
#endif

/* 默认每英寸的像素数量，用于初始化默认大小（不是很重要），例如小部件大小，样式填充 */
#define LV_DPI_DEF                          130     /* [像素/英寸] */



/*********************************************************************************

                                        特征选项
                                        
 ***********************************************************************************/

/*-------------
 * 1. 绘制
 *-----------*/

/* 启用复杂的绘制引擎
 * 需要绘制阴影，梯度，圆角，圆，弧，斜线，图像转换或任何蒙版 */
#define LV_DRAW_COMPLEX                     1

#if LV_DRAW_COMPLEX != 0

    /* 允许缓冲一些阴影计算
     * LV_SHADOW_CACHE_SIZE为最大的缓冲大小，缓冲大小为 `阴影宽度 + 半径`
     * 将会有 LV_SHADOW_CACHE_SIZE^2 的内存开销 */
    #define LV_SHADOW_CACHE_SIZE            0

    /* 设置最大缓存循环数据的数量。
     * 保存1/4圆的周长用于抗锯齿
     * 半径*每个圆使用4个字节(保存最常用的半径)
     * 0:禁用缓存 */
    #define LV_CIRCLE_CACHE_SIZE            4
    
#endif

/* 默认图像缓存大小。图像缓存保持图像打开。
 * 如果只使用内置的图像格式，缓存没有真正的优势。(即没有添加新的图像解码器)
 * 复杂的图像解码器(如PNG或JPG)缓存可以保存连续打开/解码的图像。然而，打开的图像会消耗额外的RAM。
 * 0:禁用缓存 */
#define LV_IMG_CACHE_DEF_SIZE               0

/* 每个梯度允许停止的数量。
 * 每个额外的停止增加(sizeof(lv_color_t) + 1)字节 */
#define LV_GRADIENT_MAX_STOPS               2

/* 默认梯度缓冲区大小。
 * 当LVGL计算梯度“映射”，它可以将它们保存到缓存，以避免再次计算它们。
 * LV_GRAD_CACHE_DEF_SIZE设置缓存的大小(以字节为单位)。
 * 如果缓存太小，则只在需要绘制时才分配。
 * 0表示没有缓存*/
#define LV_GRAD_CACHE_DEF_SIZE              0

/* 允许抖动渐变(在有限的颜色深度显示上实现视觉平滑的颜色渐变)
 * LV_DITHER_GRADIENT意味着在对象的渲染表面中再分配一两行
 * 如果使用错误扩散，内存消耗的增加为(32位*对象宽度)加上24位*对象宽度 */
#define LV_DITHER_GRADIENT                  0

#if LV_DITHER_GRADIENT
    /* 增加了错误扩散抖动的支持。
     * 错误扩散抖动得到了更好的视觉效果，但在绘制时意味着更多的CPU和内存消耗。内存消耗增加(24位*对象的宽度) */
    #define LV_DITHER_ERROR_DIFFUSION       0
#endif

/* 为旋转分配的最大缓冲区大小。仅在显示驱动程序中启用软件旋转时使用 */
#define LV_DISP_ROT_MAX_BUF                 (10*1024)

/*-------------
 * 2. GPU
 *-----------*/

/* 使用STM32的DMA2D(又名Chrom Art) GPU */
#define LV_USE_GPU_STM32_DMA2D              0

#if LV_USE_GPU_STM32_DMA2D
    /* 必须定义包括目标处理器的CMSIS头的路径，如："stm32f769xx.h"或"stm32f429xx.h"*/
    #define LV_GPU_DMA2D_CMSIS_INCLUDE  "stm32f429xx.h"
#endif

/* 使用NXP的PXP GPU iMX RTxxx平台 */
#define LV_USE_GPU_NXP_PXP                  0

#if LV_USE_GPU_NXP_PXP
    /* 1：为 PXP（lv_gpu_nxp_pxp_osa.c）添加默认裸机和 FreeRTOS 中断处理例程 
     * 并在lv_init()期间自动调用 lv_gpu_nxp_pxp_init()。请注意，符号SDK_OS_FREE_RTOS
     * 必须定义才能使用 FreeRTOS OSA，否则选择裸机实现。
     * 0：lv_gpu_nxp_pxp_init()必须在lv_init() 之前手动调用
     */
    #define LV_USE_GPU_NXP_PXP_AUTO_INIT    0
#endif

/* 使用NXP的VG-Lite GPU iMX RTxxx平台 */
#define LV_USE_GPU_NXP_VG_LITE              0

/* 使用SDL渲染器API */
#define LV_USE_GPU_SDL                      0

#if LV_USE_GPU_SDL
    #define LV_GPU_SDL_INCLUDE_PATH <SDL2/SDL.h>
    /* 纹理缓存大小，默认8MB */
    #define LV_GPU_SDL_LRU_SIZE (1024 * 1024 * 8)
    /* 用于蒙版绘制的自定义混合模式，如果需要使用较旧的SDL2库，请禁用 */
    #define LV_GPU_SDL_CUSTOM_BLEND_MODE (SDL_VERSION_ATLEAST(2, 0, 6))
#endif

/*-------------
 * 3. 日志
 *-----------*/

/* 启用日志模块 */
#define LV_USE_LOG                          1

#if LV_USE_LOG

    /* 如何添加重要日志:
     * LV_LOG_LEVEL_TRACE       记录详细信息（大量日志，不建议）
     * LV_LOG_LEVEL_INFO        记录重要事件
     * LV_LOG_LEVEL_WARN        记录一些不想发生的事情（没有引起问题）
     * LV_LOG_LEVEL_ERROR       记录只有在系统可能出现故障时才会出现关键问题
     * LV_LOG_LEVEL_USER        仅用户自己添加的日志
     * LV_LOG_LEVEL_NONE        不要记录任何内容
     */
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

    /*
     * 1: 使用'printf'打印日志;
     * 0: 用户需要用' lv_log_register_print_cb() '注册回调函数 
     */
    #define LV_LOG_PRINTF                   1

    /* 在产生大量日志的模块中启用/禁用LV_LOG_TRACE 这一个是具体的日志打印*/
    #define LV_LOG_TRACE_MEM                1
    #define LV_LOG_TRACE_TIMER              1
    #define LV_LOG_TRACE_INDEV              1
    #define LV_LOG_TRACE_DISP_REFR          1
    #define LV_LOG_TRACE_EVENT              1
    #define LV_LOG_TRACE_OBJ_CREATE         1
    #define LV_LOG_TRACE_LAYOUT             1
    #define LV_LOG_TRACE_ANIM               1

#endif

/*-------------
 * 4. 断言
 *-----------*/

/* 如果操作失败或发现无效数据，则启用断言。
 * 如果启用了LV_USE_LOG，失败时会打印错误信息*/
#define LV_USE_ASSERT_NULL                  1   /* 检查参数是否为NULL */
#define LV_USE_ASSERT_MALLOC                1   /* 检查内存是否分配成功 */
#define LV_USE_ASSERT_STYLE                 0   /* 检查样式是否正确初始化 */
#define LV_USE_ASSERT_MEM_INTEGRITY         0   /* 关键操作完成后，请检查“lv_mem”的完整性 */
#define LV_USE_ASSERT_OBJ                   0   /* 检查对象的类型和存在(例如，未删除) */

/* 当断言发生时，添加一个自定义处理程序，例如重新启动MCU */
#define LV_ASSERT_HANDLER_INCLUDE           <stdint.h>
#define LV_ASSERT_HANDLER while(1);

/*-------------
 * 5. 其他
 *-----------*/

/* 1:显示CPU使用率和帧率（FPS） */
#define LV_USE_PERF_MONITOR                 0

#if LV_USE_PERF_MONITOR
    #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
#endif

/* 1：显示使用的内存和内存碎片
 * 当使用LVGL自带的内存管理算法时才有用 */
#define LV_USE_MEM_MONITOR                  0

#if LV_USE_MEM_MONITOR
    #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
#endif

/* 1:在重新绘制的区域上绘制随机的彩色矩形 */
#define LV_USE_REFR_DEBUG                   0

/* 改变内置的(v)snprintf函数 */
#define LV_SPRINTF_CUSTOM                   0

#if LV_SPRINTF_CUSTOM
    #define LV_SPRINTF_INCLUDE  <stdio.h>
    #define lv_snprintf         snprintf
    #define lv_vsnprintf        vsnprintf
#else   
    #define LV_SPRINTF_USE_FLOAT            0
#endif 

#define LV_USE_USER_DATA                    1

/* 垃圾收集器设置
 * 如果lvgl绑定到高级语言，并且内存由该语言管理时使用*/
#define LV_ENABLE_GC                        0

#if LV_ENABLE_GC != 0
    #define LV_GC_INCLUDE "gc.h"
#endif 

 
 
/*********************************************************************************

                                        编译器设置
                                        
 ***********************************************************************************/
/* 对于设置为1的大端序系统 */
#define LV_BIG_ENDIAN_SYSTEM                0

/* 为' lv_tick_inc '函数定义一个自定义属性 */
#define LV_ATTRIBUTE_TICK_INC

/* 为' lv_timer_handler '函数定义一个自定义属性 */
#define LV_ATTRIBUTE_TIMER_HANDLER

/* 为' lv_disp_flush_ready '函数定义一个自定义属性 */
#define LV_ATTRIBUTE_FLUSH_READY

/* 缓冲区所需的对齐大小 */
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE         1

/* 将被添加到需要对齐内存的地方(默认情况下-Os数据可能不会对齐到边界)。
 * 如__attribute__((对齐(4))) */
#define LV_ATTRIBUTE_MEM_ALIGN

/* 属性来标记大型常量数组，例如字体的位图 */
#define LV_ATTRIBUTE_LARGE_CONST

/* RAM中大数组声明的编译器前缀 */
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY

/* 将性能关键功能放入更快的内存中(例如RAM) */
#define LV_ATTRIBUTE_FAST_MEM

/* 在GPU加速操作中使用的前缀变量，通常需要放置在DMA可访问的RAM段中 */
#define LV_ATTRIBUTE_DMA

/* 导出整型常量到绑定。该宏与LV_<CONST> that形式的常量一起使用
 * 也应该出现在LVGL绑定API，如Micropython。*/
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning /* 默认值只是防止GCC警告 */

/* 扩展默认值-32k..32k坐标范围到-4M..使用int32_t而不是int16_t作为坐标 */
#define LV_USE_LARGE_COORD                  0



/*********************************************************************************

                                        字库设置
                                        
 ***********************************************************************************/

/* ASCII范围的蒙特塞拉特字体和一些使用 bpp = 4 的符号
 * https://fonts.google.com/specimen/Montserrat */
#define LV_FONT_MONTSERRAT_8                0
#define LV_FONT_MONTSERRAT_10               1
#define LV_FONT_MONTSERRAT_12               1
#define LV_FONT_MONTSERRAT_14               1
#define LV_FONT_MONTSERRAT_16               1
#define LV_FONT_MONTSERRAT_18               0
#define LV_FONT_MONTSERRAT_20               0
#define LV_FONT_MONTSERRAT_22               1
#define LV_FONT_MONTSERRAT_24               0
#define LV_FONT_MONTSERRAT_26               0
#define LV_FONT_MONTSERRAT_28               0
#define LV_FONT_MONTSERRAT_30               0
#define LV_FONT_MONTSERRAT_32               1
#define LV_FONT_MONTSERRAT_34               0
#define LV_FONT_MONTSERRAT_36               0
#define LV_FONT_MONTSERRAT_38               0
#define LV_FONT_MONTSERRAT_40               0
#define LV_FONT_MONTSERRAT_42               0
#define LV_FONT_MONTSERRAT_44               0
#define LV_FONT_MONTSERRAT_46               0
#define LV_FONT_MONTSERRAT_48               0

/* 展示特色 */
#define LV_FONT_MONTSERRAT_12_SUBPX         0
#define LV_FONT_MONTSERRAT_28_COMPRESSED    0  /* bpp = 3 */
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW    0  /* 希伯来语，阿拉伯语，波斯语以及它们的各种形式 */
#define LV_FONT_SIMSUN_16_CJK               0  /* 1000个最常见的CJK自由基 */

/* 像素完美的等宽字体 */
#define LV_FONT_UNSCII_8                    0
#define LV_FONT_UNSCII_16                   0

/* 声明自定义字体。也可以使用这些字体作为默认字体
 * 如：#define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(my_font_1) */
#define LV_FONT_CUSTOM_DECLARE 

/* 设置默认字体 */
#define LV_FONT_DEFAULT                     &lv_font_montserrat_14

/* 允许处理大字体或包含大量字符的字体
 * 限制取决于字体大小、字体和 bpp
 * 如果字体需要，将触发编译器错误 */
#define LV_FONT_FMT_TXT_LARGE               0

/* 启用/禁用对压缩字体的支持 */
#define LV_USE_FONT_COMPRESSED              0

/* 启用子像素渲染 */
#define LV_USE_FONT_SUBPX                   0
#if LV_USE_FONT_SUBPX
    /* 设置显示器的像素顺序，RGB通道的物理顺序。对于“正常”字体无关紧要 */
    #define LV_FONT_SUBPX_BGR               0  /* 0: RGB;1: BGR */
#endif

/*********************************************************************************

                                        文本设置
                                        
 ***********************************************************************************/

/**
 * 为字符串选择字符编码
 * IDE或编辑器应该具有相同的字符编码
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/* 可以在这些字符上中断(换行)文本 */
#define LV_TXT_BREAK_CHARS                  " ,.;:-_"

/* 如果一个单词至少有这么长，就会在最好的地方断开
 * 要禁用，设置一个值<= 0 */
#define LV_TXT_LINE_BREAK_LONG_LEN          0

/* 在一个长单词中，在停顿之前放置一行的最小字符数。
 * 取决于LV_TXT_LINE_BREAK_LONG_LEN */
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN  3

/* 在一个长单词中，在停顿后插入一行的最小字符数。
 * 取决于LV_TXT_LINE_BREAK_LONG_LEN */
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/* 用于信令文本重新上色的控制字符 */
#define LV_TXT_COLOR_CMD                    "#"

/* 支持双向文本。允许混合从左到右和从右到左的文本。
 * 方向会根据Unicode双向算法进行处理:
 * https://www.w3.org/International/articles/inline-bidi-markup/uba-basics*/
#define LV_USE_BIDI                         0

#if LV_USE_BIDI
    /* 设置默认方向。支持的值:
    *`LV_BASE_DIR_LTR` 从左到右
    *`LV_BASE_DIR_RTL` 从右到左
    *`LV_BASE_DIR_AUTO` 检测文本基本方向 */
    #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
#endif

/* 支持阿拉伯语/波斯处理
 * 在这些语言中，字符应该根据其在文本中的位置被替换为其他形式 */
#define LV_USE_ARABIC_PERSIAN_CHARS         0



/*********************************************************************************

                                        核心控件设置
                                        
 ***********************************************************************************/
/* 小部件的文档:https://docs.lvgl.io/latest/en/html/widgets/index.html */

#define LV_USE_ARC                          1

#define LV_USE_ANIMIMG                      1

#define LV_USE_BAR                          1

#define LV_USE_BTN                          1

#define LV_USE_BTNMATRIX                    1

#define LV_USE_CANVAS                       1

#define LV_USE_CHECKBOX                     1

#define LV_USE_DROPDOWN                     1   /* 依赖: lv_label */

#define LV_USE_IMG                          1   /* 依赖: lv_label */

#define LV_USE_LABEL                        1
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION         1  /* 启用标签的选择文本*/
    #define LV_LABEL_LONG_TXT_HINT          1  /* 在标签中存储一些额外的信息，以加快绘制非常长的文本 */
#endif

#define LV_USE_LINE                         1

#define LV_USE_ROLLER                       1  /* 依赖: lv_label */
#if LV_USE_ROLLER
    #define LV_ROLLER_INF_PAGES             7  /* 当滚筒无限时，额外的“页数” */
#endif

#define LV_USE_SLIDER                       1  /* 依赖: lv_bar*/

#define LV_USE_SWITCH                       1

#define LV_USE_TEXTAREA                     1  /* 依赖: lv_label*/
#if LV_USE_TEXTAREA != 0
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME   1500    /*ms*/
#endif

#define LV_USE_TABLE                        1


/*********************************************************************************

                                        特别功能
                                        
 ***********************************************************************************/
/*-------------
 * 1.拓展控件
 *------------*/
#define LV_USE_CALENDAR                     1

#if LV_USE_CALENDAR
    #define LV_CALENDAR_WEEK_STARTS_MONDAY 0
    #if LV_CALENDAR_WEEK_STARTS_MONDAY
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
    #else
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
    #endif

    #define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
    #define LV_USE_CALENDAR_HEADER_ARROW    1
    #define LV_USE_CALENDAR_HEADER_DROPDOWN 1
#endif  

#define LV_USE_CHART                        1

#define LV_USE_COLORWHEEL                   1

#define LV_USE_IMGBTN                       1

#define LV_USE_KEYBOARD                     1

#define LV_USE_LED                          1

#define LV_USE_LIST                         1

#define LV_USE_MENU                         1

#define LV_USE_METER                        1

#define LV_USE_MSGBOX                       1

#define LV_USE_SPINBOX                      1

#define LV_USE_SPINNER                      1

#define LV_USE_TABVIEW                      1

#define LV_USE_TILEVIEW                     1

#define LV_USE_WIN                          1

#define LV_USE_SPAN                         1
#if LV_USE_SPAN

    #define LV_SPAN_SNIPPET_STACK_SIZE      64    /* 一个行文本可以包含最大数量的span描述符 */
#endif

/*-----------
 * 2. 主题
 *----------*/

/* 一个非常完整的主题 */
#define LV_USE_THEME_DEFAULT                1
#if LV_USE_THEME_DEFAULT

    /* 0:浅色模式，1:深色模式 */
    #define LV_THEME_DEFAULT_DARK           0

    /* 1:启用按下增长 */
    #define LV_THEME_DEFAULT_GROW           1

    /* 默认转换时间[ms] */
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif 

/* 一个非常简单的主题，这是自定义主题的良好起点 */
#define LV_USE_THEME_BASIC                  1

/* 专为单色显示器设计的主题 */
#define LV_USE_THEME_MONO                   1

/*-----------
 * 3. 布局
 *----------*/

/* 类似于CSS中的Flexbox布局 */
#define LV_USE_FLEX                         1

/* 类似于CSS中的网格布局 */
#define LV_USE_GRID                         1

/*---------------------
 * 4. 第三方库
 *--------------------*/

/* 常见API的文件系统接口 */
 
/* STDIO */
#define LV_USE_FS_STDIO            0
#if LV_USE_FS_STDIO
    #define LV_FS_STDIO_LETTER      '\0'        /* 设置驱动器可访问的大写字母(例如。“A”) */
    #define LV_FS_STDIO_PATH        ""          /* 设置工作目录。文件/目录路径将附加到它 */
    #define LV_FS_STDIO_CACHE_SIZE  0           /* >0则在lv_fs_read()中缓存这个字节数 */
#endif

/* POSIX */
#define LV_USE_FS_POSIX             0
#if LV_USE_FS_POSIX
    #define LV_FS_POSIX_LETTER      '\0'        /* 设置驱动器可访问的大写字母(例如。“A”) */
    #define LV_FS_POSIX_PATH        ""          /* 设置工作目录。文件/目录路径将附加到它 */
    #define LV_FS_POSIX_CACHE_SIZE  0           /* >0则在lv_fs_read()中缓存这个字节数 */
#endif

/* WIN32 */
#define LV_USE_FS_WIN32             0
#if LV_USE_FS_WIN32
    #define LV_FS_WIN32_LETTER      '\0'        /* 设置驱动器可访问的大写字母(例如。“A”) */
    #define LV_FS_WIN32_PATH        ""          /* 设置工作目录，文件/目录路径将附加到它 */
    #define LV_FS_WIN32_CACHE_SIZE  0           /* >0则在lv_fs_read()中缓存这个字节数 */
#endif

/* FATFS */
#define LV_USE_FS_FATFS             0
#if LV_USE_FS_FATFS
    #define LV_FS_FATFS_LETTER      '0'         /* 设置驱动器可访问的大写字母(例如。“A”) */
    #define LV_FS_FATFS_CACHE_SIZE  0           /* >0则在lv_fs_read()中缓存这个字节数 */
#endif

/* PNG译码器库 */
#define LV_USE_PNG                          0

/* BMP 译码器库 */
#define LV_USE_BMP                          0

/* JPG +分割JPG解码器库。
 * Split JPG是为嵌入式系统优化的自定义格式 */
#define LV_USE_SJPG                         0

/* GIF译码器库 */
#define LV_USE_GIF                          0

/* QR译码器库 */
#define LV_USE_QRCODE                       0

/* FreeType库 */
#define LV_USE_FREETYPE                     0
#if LV_USE_FREETYPE
    /* FreeType用于缓存字符[bytes]的内存(-1:没有缓存) */
    #define LV_FREETYPE_CACHE_SIZE          (16 * 1024)
    #if LV_FREETYPE_CACHE_SIZE >= 0
        /* 1:位图cache使用sbit cache, 0:位图cache使用图像cache
         * sbit缓存:对于小的位图(字体大小< 256)，它的内存效率更高
         *如果字体大小>= 256，必须配置为图像缓存 */
        #define LV_FREETYPE_SBIT_CACHE      0
        /* 由这个缓存实例管理的打开的FT_Face/FT_Size对象的最大数量。*／
           (0:使用系统默认值) */
        #define LV_FREETYPE_CACHE_FT_FACES  0
        #define LV_FREETYPE_CACHE_FT_SIZES  0
    #endif
#endif

/* Rlottie 库 */
#define LV_USE_RLOTTIE                      0

/* FFmpeg库的图像解码和播放视频
 * 支持所有主要的图像格式，所以不启用其他图像解码器与它t*/
#define LV_USE_FFMPEG                       0

#if LV_USE_FFMPEG
    /* 将输入信息转储到stderr */
    #define LV_FFMPEG_AV_DUMP_FORMAT        0
#endif

/*-----------
 * 5. 其他
 *----------*/

/* 1:启用API对对象进行快照 */
#define LV_USE_SNAPSHOT                     1

/* 1:使能Monkey测试 */
#define LV_USE_MONKEY                       0

/* 1:启用网格导航 */
#define LV_USE_GRIDNAV                      0

/*********************************************************************************

                                        实例
                                        
 ***********************************************************************************/

#define LV_BUILD_EXAMPLES                   1   /* 允许用库构建示例 */

/*===================
 * 演示使用
 ====================*/

/* 部件显示实例。对于内存要求较高 */
#define LV_USE_DEMO_WIDGETS                 0
#if LV_USE_DEMO_WIDGETS
#define LV_DEMO_WIDGETS_SLIDESHOW           0
#endif

/* 编码器和键盘使用实例 */
#define LV_USE_DEMO_KEYPAD_AND_ENCODER      0

/* 基准系统实例 */
#define LV_USE_DEMO_BENCHMARK               0

/* LVGL压力测试实例 */
#define LV_USE_DEMO_STRESS                  1

/* 音乐播放器实例 */
#define LV_USE_DEMO_MUSIC                   0

#if LV_USE_DEMO_MUSIC
# define LV_DEMO_MUSIC_SQUARE               0
# define LV_DEMO_MUSIC_LANDSCAPE            0
# define LV_DEMO_MUSIC_ROUND                0
# define LV_DEMO_MUSIC_LARGE                0
# define LV_DEMO_MUSIC_AUTO_PLAY            0

#endif

#endif

#endif
```

