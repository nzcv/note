## 内核层dump

```c
#include <asm/fcntl.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/uaccess.h>
#include <linux/dcache.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

static ulong start = 0x3f80000;
static int pid = 1361;
static ulong end = 0x3f90000;

static int __init dump_init(void) {
  ulong size = end - start;
  struct task_struct *tsk;
  char *vaddr;
  int retval = 0;
  int ret;

  pgd_t *pgd = NULL;
  pud_t *pud = NULL;
  pmd_t *pmd = NULL;
  pte_t *pte = NULL;  
  struct page *page;

  for_each_process(tsk) {
     if (tsk->pid == pid) {       
       break;
     }
  }

  pgd = pgd_offset(tsk->mm, start);
  if (pgd_none(*pgd)) {
    printk("not mapped in pgd\n");
    return -1;
  }

  pud = pud_offset((pgd_t *)pgd, start);
  if (pud_none(*pud)) {
    printk("not mapped in pud\n");
    return -1;
  }

  pmd = pmd_offset(pud, start);
  if (pmd_none(*pmd)) {
    printk("not mapped in pmd\n");
    return -1;
  }

  pte = pte_offset_kernel(pmd, start);
  if (pte_none(*pte)) {
    printk("not mapped in pte\n");
    return -1;
  }
  page = pte_page(*pte);
  vaddr = page_address(page);
  
  do
  {
    struct file *fp;
    mm_segment_t old_fs;
    loff_t pos;
    fp = filp_open("/data/local/tmp/dump.bin", O_RDWR | O_CREAT, 0644);
    if (IS_ERR(fp)) {
      ret = PTR_ERR(fp);
      printk(KERN_INFO "/data/local/tmp/dump.bin = %d\n", ret);
      return ret;
    }
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    pos = fp->f_pos;
    vfs_write(fp, vaddr, size, &pos);
    fp->f_pos = pos;
    set_fs(old_fs);
    filp_close(fp, NULL);
  } while(0);
  return 0;
}

static void __exit dump_exit(void) {
  printk("dump exit");
}

module_init(dump_init);
module_exit(dump_exit);
MODULE_LICENSE("GPL");
```