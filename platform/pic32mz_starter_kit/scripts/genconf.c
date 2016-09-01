#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>


int main(int argc, char **argv)
{
    config_t cfg;
    config_setting_t *setting;
    const char *str;
    int uart_speed;
    
    config_init(&cfg);
    
    /* Read the file. If there is an error, report it and exit. */
    if(! config_read_file(&cfg, "../samples_xml/sample.cfg"))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return(EXIT_FAILURE);
    }
    
    /* Get the store name. */
    setting = config_lookup(&cfg, "system");
    if(!setting)
        fprintf(stderr, "System group not found.\n");

    if (!config_setting_lookup_int(setting, "uart_speed", &uart_speed)){
        printf("uart_speed not found.\n");
    }
        
        printf("%d %d\n", uart_speed,  config_setting_length(setting));
    
         
#if 0    
    /* Output a list of all books in the inventory. */
    setting = config_lookup(&cfg, "inventory.books");
    if(setting != NULL)
    {
        int count = config_setting_length(setting);
        int i;
        
        printf("%-30s  %-30s   %-6s  %s\n", "TITLE", "AUTHOR", "PRICE", "QTY");
        
        for(i = 0; i < count; ++i)
        {
            config_setting_t *book = config_setting_get_elem(setting, i);
            
            /* Only output the record if all of the expected fields are present. */
            const char *title, *author;
            double price;
            int qty;
            
            if(!(config_setting_lookup_string(book, "title", &title)
                && config_setting_lookup_string(book, "author", &author)
                && config_setting_lookup_float(book, "price", &price)
                && config_setting_lookup_int(book, "qty", &qty)))
                continue;
            
            printf("%-30s  %-30s  $%6.2f  %3d\n", title, author, price, qty);
        }
        putchar('\n');
    }
    
    /* Output a list of all movies in the inventory. */
    setting = config_lookup(&cfg, "inventory.movies");
    if(setting != NULL)
    {
        unsigned int count = config_setting_length(setting);
        unsigned int i;
        
        printf("%-30s  %-10s   %-6s  %s\n", "TITLE", "MEDIA", "PRICE", "QTY");
        for(i = 0; i < count; ++i)
        {
            config_setting_t *movie = config_setting_get_elem(setting, i);
            
            /* Only output the record if all of the expected fields are present. */
            const char *title, *media;
            double price;
            int qty;
            
            if(!(config_setting_lookup_string(movie, "title", &title)
                && config_setting_lookup_string(movie, "media", &media)
                && config_setting_lookup_float(movie, "price", &price)
                && config_setting_lookup_int(movie, "qty", &qty)))
                continue;
            
            printf("%-30s  %-10s  $%6.2f  %3d\n", title, media, price, qty);
        }
        putchar('\n');
    }
#endif    
    config_destroy(&cfg);
    return(EXIT_SUCCESS);
}

/* eof */
