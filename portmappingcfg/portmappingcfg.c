/**
 * @file portmappingcfg.c
 * @brief Implement Port Mapping config API
 * @details Function to get and save port mapping rules to config file
 * Config file will be stored in JSON format.
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "portmappingcfg.h"

/** Config file path */
#define PF_CONFIG_FILE "routerupnp_cfg.json"

/**
 * @brief create a new Port mapping config file
 * @details create a new port mapping config file with default value 
 * if it doesn't exist.
 *
 * @return 0 if OK and < 0 if failed
 */
static int create_new_configfile()
{
    FILE *fd = fopen(PF_CONFIG_FILE, "w");
    if (fd)
    {
        cJSON *root;
        cJSON *rules;
        root = cJSON_CreateObject();
        rules = cJSON_CreateArray();
        cJSON_AddFalseToObject(root, "enable");
        cJSON_AddItemToObject(root, "rules", rules);
        char *str = cJSON_Print(root);
        cJSON_Delete(rules);
        cJSON_Delete(root);
        free(str);
        fclose(fd);
        return 0;
    }
    return -1;
}

static char *read_file(const char *filename)
{
    FILE *file = NULL;
    long length = 0;
    char *content = NULL;
    size_t read_chars = 0;

    /* open in read binary mode */
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        goto cleanup;
    }

    /* get the length */
    if (fseek(file, 0, SEEK_END) != 0)
    {
        goto cleanup;
    }
    length = ftell(file);
    if (length < 0)
    {
        goto cleanup;
    }
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        goto cleanup;
    }

    /* allocate content buffer */
    content = (char *)malloc((size_t)length + sizeof(""));
    if (content == NULL)
    {
        goto cleanup;
    }

    /* read the file into memory */
    read_chars = fread(content, sizeof(char), (size_t)length, file);
    if ((long)read_chars != length)
    {
        free(content);
        content = NULL;
        goto cleanup;
    }
    content[read_chars] = '\0';

cleanup:
    if (file != NULL)
    {
        fclose(file);
    }

    return content;
}

PortMappingCfg_t PMCFG_getConfig()
{
    PortMappingCfg_t tmp;
    char *content = read_file(PF_CONFIG_FILE);

    if (content)
    {
        int i;
        cJSON *root = cJSON_Parse(content);
        free(content);
        cJSON *enable_json = cJSON_GetObjectItem(root, "enable");
        if (cJSON_IsBool(enable_json))
        {
            tmp.is_enable = cJSON_IsTrue(enable_json);
        }
        cJSON *rules = cJSON_GetObjectItem(root, "rules");
        if (cJSON_IsArray(rules))
        {
            int arr_size = cJSON_GetArraySize(rules);
            tmp.numofrules = arr_size;
            MappingRule_t *map = (MappingRule_t *)malloc(arr_size * sizeof(MappingRule_t));
            cJSON *rule_item;
            for (i = 0; i < arr_size; i++)
            {
                rule_item = cJSON_GetArrayItem(rules, i);
                strcpy(map[i].eport, (cJSON_GetObjectItem(rule_item, "eport"))->valuestring);
                strcpy(map[i].iport, (cJSON_GetObjectItem(rule_item, "iport"))->valuestring);
                if (strcmp("UDP", (cJSON_GetObjectItem(rule_item, "proto"))->valuestring) == 0)
                {
                    map[i].proto = UDP;
                }
                else
                {
                    map[i].proto = TCP;
                }
            }
            tmp.rules = map;
        }
        if (root != NULL)
        {
            cJSON_Delete(root);
        }
    }
    else
    {
        //if file does not exist, create it
        create_new_configfile();
        tmp.is_enable = 0;
        tmp.numofrules = 0;
        tmp.rules = NULL;
    }
    return tmp;
}
int PMCFG_saveConfig(PortMappingCfg_t *pm_cfg)
{
    int i;
    FILE *fd = fopen(PF_CONFIG_FILE, "w");
    if (fd)
    {
        cJSON *root;
        cJSON *rules;
        cJSON *rule_item;
        root = cJSON_CreateObject();
        rules = cJSON_CreateArray();
        cJSON_AddBoolToObject(root, "enable", pm_cfg->is_enable);
        cJSON_AddItemToObject(root, "rules", rules);
        MappingRule_t *map = pm_cfg->rules;
        for (i = 0; i < pm_cfg->numofrules; i++)
        {
            rule_item = cJSON_CreateObject();
            cJSON_AddStringToObject(rule_item, "eport", map[i].eport);
            cJSON_AddStringToObject(rule_item, "iport", map[i].iport);
            cJSON_AddStringToObject(rule_item, "proto", get_proto_str(map[i].proto));
            cJSON_AddItemToArray(rules, rule_item);
        }
        char *str = cJSON_Print(root);
        fprintf(fd, "%s\n", str); // save data to file
        cJSON_Delete(root);
        free(str);
        fclose(fd);
        return 0;
    }
    return -1;
}
