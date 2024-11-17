# Sistema de Monitoramento de Sensores por Cômodo

Este projeto configura sensores em diferentes cômodos, armazena os dados no banco de dados **InfluxDB** e os visualiza no **Grafana**, com painéis separados por cômodo.

---

## Estrutura do Projeto

### **Cômodos e Sensores**
#### **Quarto**
- **Sensor de umidade**  
  - Dados: Umidade x Tempo  
  - Frequência de verificação: 2 segundos  
  - Frequência de salvamento: 2 segundos  
- **Sensor de temperatura**  
  - Dados: Temperatura x Tempo  
  - Frequência de verificação: 2 segundos  
  - Frequência de salvamento: 2 segundos  

#### **Cozinha**
- **Sensor de luminosidade**  
  - Dados: Intensidade luminosa x Tempo  
  - Frequência de verificação: 2 segundos  
  - Frequência de salvamento: 2 segundos  

#### **Corredor**
- **Sensor de movimento**  
  - Dados: Detecção de movimento (Função degrau: 1 = movimento detectado, 0 = sem movimento) x Tempo  

#### **Garagem**
- **Servo motor**  
  - Dados: Estado do servo motor (Função degrau: 1 = aberto, 0 = fechado) x Tempo  

---

## Estrutura dos Dados no InfluxDB

### **Banco de Dados:** `casa_sensores`
Os dados são organizados por **cômodo**, cada um com suas respectivas medições.

#### **Quarto**
- **Medições:**  
  - `umidade`  
  - `temperatura`  
- **Estrutura dos dados:**  
  - **Tags:**  
    - `comodo: quarto`  
  - **Campos:**  
    - `umidade` (float)  
    - `temperatura` (float)  
  - **Timestamp:** Captura a cada 2 segundos.  

#### **Cozinha**
- **Medições:**  
  - `luminosidade`  
- **Estrutura dos dados:**  
  - **Tags:**  
    - `comodo: cozinha`  
  - **Campos:**  
    - `luminosidade` (float)  
  - **Timestamp:** Captura a cada 2 segundos.  

#### **Corredor**
- **Medições:**  
  - `movimento`  
- **Estrutura dos dados:**  
  - **Tags:**  
    - `comodo: corredor`  
  - **Campos:**  
    - `movimento` (boolean, 1 ou 0)  
  - **Timestamp:** Captura quando houver alteração.  

#### **Garagem**
- **Medições:**  
  - `servo_motor`  
- **Estrutura dos dados:**  
  - **Tags:**  
    - `comodo: garagem`  
  - **Campos:**  
    - `servo_motor` (boolean, 1 ou 0)  
  - **Timestamp:** Captura quando houver alteração.  

---

## Painéis no Grafana

Os painéis do Grafana exibem os dados por cômodo:

### **Painel 1: Quarto**
- **Gráficos:**
  - Umidade x Tempo  
  - Temperatura x Tempo  

### **Painel 2: Cozinha**
- **Gráfico:**  
  - Intensidade luminosa x Tempo  

### **Painel 3: Corredor**
- **Gráfico:**  
  - Movimento (Função degrau: 1 = movimento detectado, 0 = sem movimento) x Tempo  

### **Painel 4: Garagem**
- **Gráfico:**  
  - Estado do servo motor (Função degrau: 1 = aberto, 0 = fechado) x Tempo  


