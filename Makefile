
NAME		= webserv
# -Wall -Wextra -Werror -std=c++98
CXXFLAGS	=  \
				-MMD -MP -g
CPPFLAGS	= -I ./include -I ./include/config -I ./include/http -I ./include/server

RM			= rm -rf

CONFIG		:=	\
 				Config \
				ConfigParser \
 				DerivTree \
 				LocationConfig \
 				ServerConfig 

HTTP		:=	\
				CgiMethodExecutor \
				DefaultMethodExecutor \
               	HttpMessage \
				HttpRequestMessage \
				HttpResponseBuilder \
				HttpResponseMessage \
				ResponseHeaderAdder \
        ServerErrors \
				Utils \
				WebservValues
        
SERVER		:=	\
 				Client \
 				Server \
				ServerHandler

CONFIG		:=	$(addprefix config/, $(CONFIG))
HTTP		:=	$(addprefix http/, $(HTTP))
SERVER		:=	$(addprefix server/, $(SERVER))

FILENAME	=	\
 				main \
				$(CONFIG)	\
				$(HTTP)		\
				$(SERVER)
				
SRCS_DIR	=	srcs
SRCS		=	$(addprefix $(SRCS_DIR)/, $(addsuffix .cpp, $(FILENAME)))

OBJS_DIR	=	objs
OBJS		=	$(addprefix $(OBJS_DIR)/, $(addsuffix .o, $(FILENAME)))

DEPS		=	$(addsuffix .d, $(FILENAME))

vpath %.cpp srcs/config srcs srcs/http srcs/server
vpath %.d objs

.PHONY: all clean fclean re

all: $(NAME)

-include $(DEPS)

$(NAME): $(OBJS)
	@$(CXX) $^ $(OUTPUT_OPTION)
	@echo "\033[32mComplete✅"

$(OBJS_DIR):
	@mkdir $(OBJS_DIR)

$(OBJS_DIR)/%.o: %.cpp | $(OBJS_DIR)
	@$(COMPILE.cpp) $< $(OUTPUT_OPTION)

clean:
	@$(RM) $(OBJS_DIR)
	@echo "\033[31mClean🧽"

fclean:
	@$(RM) $(OBJS_DIR) $(NAME)
	@echo "\033[31mfclean🧼"

re:
	@make fclean
	@make all
