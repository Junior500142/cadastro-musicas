# Cadastro de Músicas 🎵  
Sistema de gerenciamento de registros desenvolvido em **C** como trabalho da disciplina *Linguagem de Programação* (Prof. Maikon Bueno).  

Este projeto implementa um cadastro de músicas com menu interativo, armazenamento em arquivo, lista encadeada e modo servidor TCP, além de um servidor HTTP opcional.

---

## 📋 Descrição
O sistema permite gerenciar um banco de músicas com as seguintes informações:
- **Título**
- **Artista**
- **Álbum**
- **Ano**

Os dados são armazenados temporariamente em **lista encadeada** durante a execução e persistidos em arquivo binário ao encerrar.

---

## ✨ Funcionalidades
✅ **Menu interativo no terminal**  
- Adicionar nova música  
- Listar músicas cadastradas  
- Remover música por título  

✅ **Persistência de dados**  
- Leitura do arquivo ao iniciar o programa  
- Escrita no arquivo ao encerrar  

✅ **Servidor TCP**  
- Permite conexão de um cliente externo para enviar comandos:
  - `READ` → retorna todos os registros
  - `DELETE <chave>` → remove música pelo título
- Comandos de execução:
  ```bash
  ./programa server
  ./programa client <ip_do_servidor> <porta>
