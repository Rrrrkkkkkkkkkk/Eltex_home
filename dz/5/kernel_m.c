#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/timekeeping.h>
#include <linux/uaccess.h>

#define FILE_PATH "/var/log/kernel_time.log"  // Путь к файлу

static struct file *file;
static mm_segment_t old_fs;
static char buffer[128];

// Функция записи в файл
static void write_to_file(const char *message) {
    loff_t pos = 0;
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    file = filp_open(FILE_PATH, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (IS_ERR(file)) {
        printk(KERN_ERR "Ошибка открытия файла %s\n", FILE_PATH);
        set_fs(old_fs);
        return;
    }

    kernel_write(file, message, strlen(message), &pos);
    filp_close(file, NULL);
    set_fs(old_fs);
}

// Функция загрузки модуля
static int __init time_logger_init(void) {
    struct timespec64 ts;
    ktime_get_real_ts64(&ts);
    snprintf(buffer, sizeof(buffer), "Модуль загружен: %lld сек\n", ts.tv_sec);

    write_to_file(buffer);
    printk(KERN_INFO "Модуль загружен в %lld сек\n", ts.tv_sec);
    return 0;
}

// Функция выгрузки модуля
static void __exit time_logger_exit(void) {
    struct timespec64 ts;
    ktime_get_real_ts64(&ts);
    snprintf(buffer, sizeof(buffer), "Модуль выгружен: %lld сек\n", ts.tv_sec);

    write_to_file(buffer);
    printk(KERN_INFO "Модуль выгружен в %lld сек\n", ts.tv_sec);
}

module_init(time_logger_init);
module_exit(time_logger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ваше Имя");
MODULE_DESCRIPTION("Модуль, записывающий время загрузки и выгрузки");
