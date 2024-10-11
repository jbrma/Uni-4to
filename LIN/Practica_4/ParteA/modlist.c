#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/vmalloc.h>
#include <linux/ftrace.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include "list.h"

#define MAXSIZE 256

/* Estructura que representa los nodos de la lista */
struct list_item_t {
	int data;
    struct list_head links;
}list_item;

struct list_head myList; /* Nodo fantasma (cabecera) de la lista enlazada */

static spinlock_t mySpin;

static struct proc_dir_entry *proc_entry;

static ssize_t write_proc(struct file *filp, const char *buf, size_t len, loff_t *off) {//usuario-kernel
    int n;
    //char com[15]; //para guardar el comando
	char kbuf[MAXSIZE];

	if(len > MAXSIZE){
        printk(KERN_INFO "Modlist: No hay espacio\n");
		return -EINVAL;
	}

	if(copy_from_user(kbuf, buf, len))
		return -EFAULT; //error de copia desde el espacio de usuario


	kbuf[len]= '\0';//marcando final
   
     //sscanf(kbuf, "%s", com)//guarda el escaneo en com y n

    if(sscanf(kbuf, "add %i", &n) == 1){

        struct list_item_t *nodo = NULL; 

        nodo = kmalloc(sizeof(struct list_item_t), GFP_KERNEL);
        nodo->data = n;

        spin_lock(&mySpin);

        list_add_tail(&nodo->links, &myList);
        printk(KERN_INFO "Modlist: El dato %i se ha añadido a la lista\n", n);

        spin_unlock(&mySpin);

    }
    else if(sscanf(kbuf, "remove %i", &n) == 1){
        int rem; //Cantidad de nums que ha quitado
        struct list_item_t *node = NULL; 
        struct list_head *posAct = NULL; //posicion actual de la lista
        struct list_head *aux = NULL; 

        spin_lock(&mySpin);

        list_for_each_safe(posAct, aux, &myList) {
            node = list_entry(posAct, struct list_item_t, links);
			if (node->data == n) {
				list_del(&node->links); 
				kfree(node);
                printk(KERN_INFO "Modlist: El elemento %i, se ha borrado\n", n);
				rem++;	
        	}
        }
        if(rem == 0){
            printk(KERN_INFO "Modlist: La lista no contiene ese elemento");
        }

        spin_unlock(&mySpin);
    }
    else if(strncmp(kbuf, "cleanup", 6) == 0){ //trcmp(com,"cleanup") == 1
        struct list_item_t *node = NULL; 
        struct list_head *posAct = NULL; //posicion actual de la lista
        struct list_head *aux = NULL; 

        spin_lock(&mySpin);

        list_for_each_safe(posAct, aux, &myList) {
            node = list_entry(posAct, struct list_item_t, links);
            list_del(&node->links); 
            kfree(node); //libera memoria
        }
        printk(KERN_INFO "Modlist: La lista se ha vaciado");

        spin_unlock(&mySpin);

    }else{
        return -EINVAL;
    }

    off += len;

    return len;
}


static ssize_t read_proc(struct file *filp, char *buf, size_t len, loff_t *off) {//kernel-usuario
    
    int numBytes = 0, cont = 0;
    struct list_item_t *node = NULL; 
    struct list_head *posAct = NULL; //posicion actual de la lista
    struct list_head *aux = NULL;
    char kbuf[MAXSIZE];
    char *pos = kbuf;

    

    if((*off) > 0) {//e archivo ya se ha leido //evitamos lecturas innecesarias
        return 0;
    }
    
    spin_lock(&mySpin);

    list_for_each_safe(posAct, aux, &myList) {
        node = list_entry(posAct, struct list_item_t, links);
        pos += sprintf(pos,"%i\n", node->data);
        cont++;

        if (cont == MAXSIZE){
            printk(KERN_INFO "Modlist: No hay suficiente tamaño para almacerar los elementos de la lista");
            spin_unlock(&mySpin);
            return -EFAULT;
        }
    }

    spin_unlock(&mySpin);

    numBytes = (pos-kbuf);

    if (len < numBytes) return -ENOSPC;

    //Pasa a las paginas del kernel a las del userspace, len bytes de kbuf -> buf
	if (copy_to_user(buf, kbuf, numBytes)) return -EFAULT;
   
	(*off) += numBytes; //Actualiza el puntero


    return numBytes;
}

static struct proc_ops proc_fops = {
    .proc_read = read_proc,
    .proc_write = write_proc,
};

int mod_init(void) {

    proc_entry = proc_create("modlist", 0666, NULL, &proc_fops);

    spin_lock_init(&mySpin);

    if (!proc_entry) {
        printk(KERN_ERR "No se pudo crear /proc/\n");
        return -ENOMEM;
    }

     INIT_LIST_HEAD(&myList);

    printk(KERN_INFO "Módulo de kernel 'modlist' cargado\n");
    return 0;
}

void mod_exit(void) {
    proc_remove(proc_entry);
    printk(KERN_INFO "Módulo de kernel 'modlist' descargado\n");

    if(spin_is_locked(&mySpin) != 0){
        spin_unlock(&mySpin);
    }
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");