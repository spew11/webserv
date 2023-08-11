
NAME		= webserv
# -Wall -Wextra -Werror
CXXFLAGS	=  \
				-std=c++98 -MMD -MP -g
CPPFLAGS	= -I ./include -I ./include/config -I ./include/http -I ./include/server

RM			= rm -rf

CONFIG		:=	\
				ConfigParser \
 				Config \
 				ServerConfig \
 				LocationConfig \
 				DerivTree 

HTTP		:=	\
				WebservValues \
				ResponseHeaderAdder \
               	HttpMessage \
				HttpRequestMessage \
				HttpResponseMessage \
				HttpResponseBuilder \
				Utils \
				DefaultMethodExecutor \
				ResponseHeaderAdder \
				CgiMethodExecutor \
				ServerErrors \
				
SERVER		:=	\
				ServerHandler \
 				Server \
 				Client

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
