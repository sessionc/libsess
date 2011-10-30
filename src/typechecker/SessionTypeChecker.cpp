/** --SessionTypeChecker.cpp ----------------------------------------------- **/
/*                                                                            */
/*  Session type checker                                                      */
/*                                                                            */
/*  To be compiled from $LLVM_ROOT/tools/clang/examples/SessionTypeChecker/   */
/** ------------------------------------------------------------------------ **/
#include <algorithm>
#include <cstdio>
#include <deque>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "clang/AST/Decl.h"
#include "clang/AST/DeclGroup.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/AST/TypeLocVisitor.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Rewrite/ASTConsumers.h"

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/CompilerInstance.h"

#include "st_node.h"

extern "C" {
  st_node *parse(const char *filename);
}

using namespace clang;

namespace {

  /**
   * Custom AST Consumer.
   *  (1) to walk the AST and generate ST tree for code
   *  (2) to type check ST tree against Scribble ST tree
   */
  class SessionTypeCheckingConsumer :
      public ASTConsumer,
      public DeclVisitor<SessionTypeCheckingConsumer>,
      public StmtVisitor<SessionTypeCheckingConsumer>,
      public TypeLocVisitor<SessionTypeCheckingConsumer> {

    // AST-related fields.
    ASTContext    *context_;
    SourceManager *src_mgr_;
    TranslationUnitDecl *tu_decl_;

    // Local fields for building session type tree.
    st_node *root_;
    st_node *scribble_root_;
    std::stack< st_node * > appendto_node;
    std::vector<std::string> roles;
    std::vector<std::string> roles_table;

    int caseState, ifState;
    int breakStmt_count, branch_count, outbranch_count, chain_count;
    int thenOrElse; // 1 or 0

    // Local variables for Visitors.
    Decl *current_decl_;
    typedef DeclVisitor<SessionTypeCheckingConsumer> BaseDeclVisitor;
    typedef StmtVisitor<SessionTypeCheckingConsumer> BaseStmtVisitor;
    typedef TypeLocVisitor<SessionTypeCheckingConsumer> BaseTypeLocVisitor;

    public:
      virtual void Initialise(ASTContext &ctx, std::string &scribble_filename) {
        context_ = &ctx;
        src_mgr_ = &context_->getSourceManager();
        tu_decl_ = context_->getTranslationUnitDecl();

        // Parse the Scribble file.
        scribble_root_ = parse((const char *)scribble_filename.c_str());

        if (scribble_root_ == NULL) { // ie. parse failed
          fprintf(stderr, "ERROR: Unable to parse Scribble file.\n");
          return;
        }

        // Session Type tree root.
        root_ = (st_node *)malloc(sizeof(st_node));
        // TODO: call this after session-initiation function call.
        init_st_node(root_, BEGIN_NODE, "", "");

        chain_count = 0;
        caseState = 0;
        ifState = 0;
        breakStmt_count = 0;
        branch_count = 0;
        outbranch_count = 0;

      } // Initialise()

      /**
       * Entry point to session type checker.
       * TranslationUnit is the base block of a C program.
       *
       */
      void HandleTranslationUnit(ASTContext &ctx) {
        TranslationUnitDecl *tu_decl = context_->getTranslationUnitDecl();

        appendto_node.push(root_);

        for (DeclContext::decl_iterator
             iter = tu_decl->decls_begin(), iter_end = tu_decl->decls_end();
             iter != iter_end; ++iter) {
          // Walk the AST to get source code ST tree.
          Decl *decl = *iter;
          BaseDeclVisitor::Visit(decl);
        }

        // Normalise.
        normalise(root_);
        // remove_empty_branch_node(root_);
        // sort_branches(root_);

        // Type-checking.
        if (compare_st_node(root_, scribble_root_)) {

          llvm::outs() << "\n[Session Type Checker] "
                       << "Scribble description matches code\n\n";

        } else {

          llvm::outs() << "\n[Session Type Checker] "
                       << "Scribble description does NOT match code\n\n";

          // For diagnosis.
          llvm::outs() << "[AST] ST tree:\n";
          print_st_node(root_, 0);

          llvm::outs() << "[Scribble] ST tree:\n";
          print_st_node(scribble_root_, 0);

        }

        free_st_node(root_);
        free_st_node(scribble_root_);
      }

 //===--------------------------------------------------------------------===//
 // Visitors
 //===--------------------------------------------------------------------===//

      // Generic visitor.
      void Visit(Decl *decl) {
        if (!decl) return;

        Decl *prev_decl = current_decl_;
        current_decl_ = decl;
        BaseDeclVisitor::Visit(decl);
        current_decl_ = prev_decl;
      }

      // Declarator (variable | function | struct | typedef) visitor.
      void VisitDeclaratorDecl(DeclaratorDecl *decl_decl) {
        BaseDeclVisitor::VisitDeclaratorDecl(decl_decl);
        if (TypeSourceInfo *TInfo = decl_decl->getTypeSourceInfo())
          BaseTypeLocVisitor::Visit(TInfo->getTypeLoc());
      }

      // Declaration visitor.
      void VisitDecl(Decl *D) {
        if (isa<FunctionDecl>(D) || isa<ObjCMethodDecl>(D) || isa<BlockDecl>(D))
          return;

        // Generate context from declaration.
        if (DeclContext *DC = dyn_cast<DeclContext>(D))
          cast<SessionTypeCheckingConsumer>(this)->VisitDeclContext(DC);
      }

      // Declaration context visitor.
      void VisitDeclContext(DeclContext *DC) {
        for (DeclContext::decl_iterator
             iter = DC->decls_begin(), iter_end = DC->decls_end();
             iter != iter_end; ++iter) {
          Visit(*iter);
        }
      }

      // Variable visitor.
      void VisitDeclRefExpr(DeclRefExpr* expr) {
      }

      // Function (declaration and body) visitor.
      void VisitFunctionDecl(FunctionDecl *D) {
        BaseDeclVisitor::VisitFunctionDecl(D);
        if (D->isThisDeclarationADefinition()) {
            BaseStmtVisitor::Visit(D->getBody());
        }
      }

      // Generic code block (declaration and body) visitor.
      void VisitBlockDecl(BlockDecl *D) {
        BaseDeclVisitor::VisitBlockDecl(D);
        BaseStmtVisitor::Visit(D->getBody());
      }

      // Variable declaration visitor.
      void VisitVarDecl(VarDecl *D) {
        BaseDeclVisitor::VisitVarDecl(D);

        // If variable is a role, keep track of it.
        if (src_mgr_->isFromMainFile(D->getLocation())
            && D->getType().getAsString() == "role *") {

          // Not checking for duplicates - Frontend will prevent it
          roles_table.push_back(D->getNameAsString());
        }

        // Initialiser.
        if (Expr *Init = D->getInit())
          BaseStmtVisitor::Visit(Init);
      }

 //===--------------------------------------------------------------------===//
 // StmtVisitor
 //===--------------------------------------------------------------------===//

      void VisitDeclStmt(DeclStmt *stmt) {
        for (DeclStmt::decl_iterator
             iter = stmt->decl_begin(), iter_end = stmt->decl_end();
             iter != iter_end; ++iter)
          Visit(*iter);
      }

      void VisitBlockExpr(BlockExpr *expr) {
        // The BlockDecl is also visited by 'VisitDeclContext()'.
        // No need to visit it twice.
      }

      // Statement visitor.
      void VisitStmt(Stmt *stmt) {

        // Break statement inside "switch-case".
        if (isa<BreakStmt>(stmt) && caseState == 1) {
            breakStmt_count++;
        }

        // Function call statement.
        if (isa<CallExpr>(stmt)) { // FunctionCall
          CallExpr *callExpr = cast<CallExpr>(stmt);

          //
          // Order of evaluating a function CALL is different from
          // the order in the AST
          // We want to make sure the arguments are evaluated first
          // in a function call, before evaluating the func call itself
          //
    
          Stmt *func_call_stmt = NULL;
          std::string func_name(callExpr->getDirectCallee()->getNameAsString());
          std::string datatype;
          std::string role;

          for (Stmt::child_iterator
               iter = stmt->child_begin(), iter_end = stmt->child_end();
               iter != iter_end; ++iter) {

            // Skip first child (FunctionCall Stmt).
            if (func_call_stmt == NULL) {
              func_call_stmt = *iter;
              continue;
            }

            // Visit function arguments.
            if (*iter) BaseStmtVisitor::Visit(*iter);
          }

          // Visit FunctionCall Stmt. 
          BaseStmtVisitor::Visit(func_call_stmt);

          //
          // Basic assumption
          // 1. Both sending and receiving uses prefix_type_name format
          // 2. Arguments positions of sending and receiving are same
          //    (at least for role argument)
          //

          // ---------- Send ----------
          if (func_name.find("send_") != std::string::npos) {

            addtoBranch_counter();

            // Extract the datatype (last segment of function name).
            datatype = func_name.substr(func_name.find("_") + 1,
                                        std::string::npos);

            // Extract the role (first argument).
            Expr *expr = callExpr->getArg(0);
            if (ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(expr)) {
              if (DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(ICE->getSubExpr())) {
                if (VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl())) {
                  role = VD->getNameAsString();
                }
              }
            }


            if (strcmp(datatype.c_str(), "string") == 0) {
              Expr *value = callExpr->getArg(1);
              if (ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(value)) {
                if (ImplicitCastExpr *ICE2 = dyn_cast<ImplicitCastExpr>(ICE->getSubExpr())) {
                  if (StringLiteral *SL = dyn_cast<StringLiteral>(ICE2->getSubExpr())) {
                    std::string str_lit = SL->getString();
                    llvm::outs() << "[StringLiteral " << str_lit << "]\n";
                  }
                }
              }
            }

            // Construct the ST node.
            st_node *node = (st_node *)malloc(sizeof(st_node));
            init_st_node(node, SEND_NODE, role.c_str(), datatype.c_str());

            // Put new ST node in position (ie. child of previous_node).
            st_node * previous_node = appendto_node.top();
            append_st_node(previous_node, node);
                    
            return; // End of SEND_NODE construction.
          }
          // ---------- End of Send -----------


          // ---------- Receive/Recv ----------
          if (func_name.find("receive_") != std::string::npos  // Indirect recv
              || func_name.find("recv_") != std::string::npos) { // Direct recv

            addtoBranch_counter();

            // Extract the datatype (last segment of function name).
            datatype = func_name.substr(func_name.find("_") + 1,
                                        std::string::npos);

            // Extract the role (first argument).
            Expr *expr = callExpr->getArg(0);
            if (ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(expr)) {
              if (DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(ICE->getSubExpr())) {
                if (VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl())) {
                  role = VD->getNameAsString();
                }
              }
            }

            // Construct the ST node.
            st_node *node = (st_node *)malloc(sizeof(st_node));
            init_st_node(node, RECV_NODE, role.c_str(), datatype.c_str());

            // Put new ST node in position (ie. child of previous_node).
            st_node * previous_node = appendto_node.top();
            append_st_node(previous_node, node);
            
            return; // end of RECV_NODE construction.
          }
          // ---------- End of Receive/Recv ----------


          // ---------- Outwhile ----------
          if (func_name.find("outwhile") != std::string::npos) {

            addtoBranch_counter();
            chain_count++;

            // Extract the roles (varyargs, third argument to last argument).
            std::string role_str;
            for (unsigned arg = 2, arg_end = callExpr->getNumArgs();
                 arg < arg_end; ++arg) {

              // Extract the role(s).
              Expr *expr = callExpr->getArg(arg);
              if (ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(expr)) {
                if (DeclRefExpr *DRE=dyn_cast<DeclRefExpr>(ICE->getSubExpr())) {
                  if (VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl())) {
                    role = VD->getNameAsString();
                    roles.push_back(role);
                  }
                }
              }
            }

            // Create role string (joined outwhile roles).
            sort(roles.begin(), roles.end());
            for (std::vector<std::string>::iterator
                 it=roles.begin(), it_end=roles.end();
                 it < it_end; ++it) {
              role_str += *it + "|";
            }

            st_node *node = (st_node *)malloc(sizeof(st_node));
            init_st_node(node, OUTWHILE_NODE, role_str.c_str(), "");

            st_node * previous_node = appendto_node.top();
            append_st_node(previous_node, node);
            appendto_node.push(node);
            roles.clear();
                  
            return;
          }
          // ---------- End of Outwhile ----------


          // ---------- Inwhile ----------
          if (func_name.find("inwhile") != std::string::npos) {
                    
            addtoBranch_counter();
            chain_count++;

            // Extract the roles (varyargs, second argument to last argument).
            std::string role_str;
            for (unsigned arg = 1, arg_end = callExpr->getNumArgs();
                 arg < arg_end; ++arg) {

              // Extract the role(s).
              Expr *expr = callExpr->getArg(arg);
              if (ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(expr)) {
                if (DeclRefExpr *DRE=dyn_cast<DeclRefExpr>(ICE->getSubExpr())) {
                  if (VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl())) {
                    role = VD->getNameAsString();
                    roles.push_back(role);
                  }
                }
              }
            }

            // Create role string (joined inwhile roles).
            sort(roles.begin(), roles.end());
            for (std::vector<std::string>::iterator
                 it=roles.begin(), it_end=roles.end();
                 it < it_end; ++it) {
              role_str += *it + "|";
            }

            st_node *node = (st_node *)malloc(sizeof(st_node));
            init_st_node(node, INWHILE_NODE, role_str.c_str(), "");

            st_node * previous_node = appendto_node.top();
            append_st_node(previous_node, node);
            appendto_node.push(node);
            roles.clear();

            return;
          }
          // ---------- End of Inwhile ----------


          // ---------- Outbranch ----------
          if (func_name.find("outbranch") != std::string::npos) {
            std::string branchtag;
            if (addtoBranch_counter()) {
              outbranch_count++;
            }

            // Extract the role.
            Expr *expr = callExpr->getArg(0);
            if (ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(expr)) {
              if (DeclRefExpr *DRE =dyn_cast<DeclRefExpr>(ICE->getSubExpr())) {
                if (VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl())) {
                  role = VD->getNameAsString();
                }
              }
            }

            // Extract the tags.
            expr = callExpr->getArg(1);
            if (ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(expr)) {
              if (DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(ICE->getSubExpr())) {
                if (VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl())) {
                  branchtag = VD->getNameAsString(); // XXX: variable name
                  // l.getSourceLocation(VD->getSourceLocation(), NULL, src_mgr_, &context_->getLangOpts());
                }
              }
            }

            st_node *node = (st_node *)malloc(sizeof(st_node));
            init_st_node(node, OUTBRANCH_NODE, role.c_str(), "");
            strncpy(node->branchtag, branchtag.c_str(), branchtag.size() < 255 ? branchtag.size() : 254);

            st_node * previous_node = appendto_node.top();
            append_st_node(previous_node, node);
            appendto_node.push(node);

            return;
          }
          // ---------- End of Outbranch ----------


          // ---------- Inbranch ----------
          if (func_name.find("inbranch") != std::string::npos) {
            std::string branchtag;

            addtoBranch_counter();

            // Extract the role.
            Expr *expr = callExpr->getArg(0);
            if (ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(expr)) {
              if (DeclRefExpr *DRE =dyn_cast<DeclRefExpr>(ICE->getSubExpr())) {
                if (VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl())) {
                  role = VD->getNameAsString();
                }
              }
            }

            // Extract the tags.
            expr = callExpr->getArg(1);
            if (ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(expr)) {
              if (DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(ICE->getSubExpr())) {
                if (VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl())) {
                  branchtag = VD->getNameAsString(); // XXX: variable name
                  // l.getSourceLocation(VD->getSourceLocation(), NULL, src_mgr_, &context_->getLangOpts());
                }
              }
            }

            st_node *node = (st_node *)malloc(sizeof(st_node));
            init_st_node(node, INBRANCH_NODE, role.c_str(), "");

            st_node * previous_node = appendto_node.top();
            append_st_node(previous_node, node);
            appendto_node.push(node);

            return;
          }
          // ---------- End of Inbranch ----------

        } // if isa<CallExpr>


        // Switch-Case statement.
        if (isa<SwitchCase>(stmt)) {

          // Hack to see if previous "case" has a "break".
          if (caseState == 1) {
            appendto_node.pop();
            caseState = 0;
            if (breakStmt_count < 1) {
              // TODO: Use Diagnostics()
              fprintf(stderr, "Warn: break statement missing after CASE statement\n");
            }
            breakStmt_count = 0;
          }

          SwitchCase *switchCase = cast<SwitchCase>(stmt);
          st_node *appendnode = (st_node *)malloc(sizeof(st_node));
          init_st_node(appendnode, BRANCH_NODE, "", "");

          st_node * previous_node = appendto_node.top();
          append_st_node(previous_node, appendnode);
          appendto_node.push(appendnode);
                
          BaseStmtVisitor::Visit(switchCase->getSubStmt()); 
          caseState = 1;

          return;
        }


        // Switch statement.
        if (isa<SwitchStmt>(stmt)) {
          SwitchStmt *switchStmt = cast<SwitchStmt>(stmt);
          Expr *cond = switchStmt->getCond();
   
          // Detect if this is inbranch.
          if (isa<CallExpr>(cond)) {

            CallExpr *callExpr = cast<CallExpr>(cond);
            std::string func_name (callExpr->getDirectCallee()->getNameAsString());
            if (func_name.find("inbranch") != std::string::npos) {

              BaseStmtVisitor::Visit(switchStmt->getCond());
              BaseStmtVisitor::Visit(switchStmt->getBody());
                                              
              if (breakStmt_count < 1) {
                // TODO: Use Diagnostics()
                fprintf(stderr, "Warn: break statement missing after CASE statement\n");
              }

              breakStmt_count = 0;
              appendto_node.pop(); // pop up the last branch node (CASE statement)
              appendto_node.pop(); // get out of the switch block

              return;
            }
          } // isa<CallExpr>(cond)

        } // isa<SwitchStmt>


        // While statement.
        if (isa<WhileStmt>(stmt)) {
          WhileStmt *whileStmt = cast<WhileStmt>(stmt);
          Expr *cond = whileStmt->getCond();

          if (isa<CallExpr>(cond)) {	   
            CallExpr *callExpr = cast<CallExpr>(cond);      
            std::string func_name(callExpr->getDirectCallee()->getNameAsString());

            // No inwhile or outwhile, ie. simple recursion.
            if (func_name.find("outwhile") != std::string::npos
                && func_name.find("inwhile") != std::string::npos) {

              BaseStmtVisitor::Visit(whileStmt->getCond());
              BaseStmtVisitor::Visit(whileStmt->getBody());

              for (int i = 0; i < chain_count; i++)
                appendto_node.pop();
              chain_count = 0;
                    
              return;	  
            } 
          } else { // Not callExpr and callExpr not inwhile/outwhile.

            st_node *node = (st_node *)malloc(sizeof(st_node));
            init_st_node(node, RECUR_NODE, "", "");

            st_node *previous_node = appendto_node.top();
            append_st_node(previous_node, node);
            appendto_node.push(node);

            BaseStmtVisitor::Visit(whileStmt->getBody());

            appendto_node.pop();

            return;
          }

        } // isa<WhileStmt>


        // If statement.
        if (isa<IfStmt>(stmt)) { 
                
          int innerIf = 0;
          if (addtoBranch_counter()) {
            branch_count++;
            innerIf = 1;                
          }
          ifState = 1;

          IfStmt *ifStmt = cast<IfStmt>(stmt);

          // We assume there is branch by default
          // so build BRANCH_NODE (remove if not needed by normalisation).

          st_node *node = (st_node *)malloc(sizeof(st_node));
          init_st_node(node, BRANCH_NODE, "", "");

          st_node *previous_node = appendto_node.top();
          append_st_node(previous_node, node);
          appendto_node.push(node);

          // Then-block.
          if (ifStmt->getElse() != NULL) {
            BaseStmtVisitor::Visit(ifStmt->getThen());

            // If there was an outbranch
            // the parent node should be reset to this parent.
            if (outbranch_count > 0) {
             appendto_node.pop();
              outbranch_count = 0;
            }
          }

          // Else-block.
          if (ifStmt->getElse() != NULL) {
            BaseStmtVisitor::Visit(ifStmt->getElse());

            // If there was an outbranch
            // the parent node should be reset to this parent.
            if (outbranch_count > 0) {
              appendto_node.pop();
              outbranch_count = 0;
            }
          }

          if (branch_count == 0) {                    
            delete_last_st_node(previous_node);
          }

          // We can reset branch_count if
          // we are not inside a nested if.
          if (innerIf == 0) {
            branch_count = 0;
            ifState = 0;
          }

          appendto_node.pop();

          return;	 
 
        } // isa<IfStmt>

        for (Stmt::child_iterator
             iter = stmt->child_begin(), iter_end = stmt->child_end();
             iter != iter_end; ++iter) {
          if (*iter) {
            BaseStmtVisitor::Visit(*iter);
          }
        }
      }


      // Add to branch counter if it is inside the IF statement block (including THEN and ELSE)
      int addtoBranch_counter() {
        if (ifState == 1) {
          branch_count++;
          return 1;
        }
        return 0;
      }

 //===--------------------------------------------------------------------===//
 // TypeLocVisitor
 //===--------------------------------------------------------------------===//

      void Visit(TypeLoc TL) {
        for (; TL; TL = TL.getNextTypeLoc()) {
          BaseTypeLocVisitor::Visit(TL);
        }
      }

      void VisitArrayLoc(ArrayTypeLoc TL) {
        BaseTypeLocVisitor::VisitArrayTypeLoc(TL);
        if (TL.getSizeExpr()) {
          BaseStmtVisitor::Visit(TL.getSizeExpr());
        }
      }

      void VisitFunctionTypeLoc(FunctionTypeLoc TL) {
        BaseTypeLocVisitor::VisitFunctionTypeLoc(TL);
        for (unsigned i = 0; i != TL.getNumArgs(); ++i) {
          BaseDeclVisitor::Visit(TL.getArg(i));
        }
      }

    // end public:

  }; // class SessionTypeCheckingConsumer


  /**
   * Toplevel Plugin interface to dispatch type checker.
   *
   */
  class SessionTypeCheckingAction : public PluginASTAction {
    private:
      std::string scribble_filename;

    protected:
      ASTConsumer *CreateASTConsumer(CompilerInstance &CI, llvm::StringRef) {
        SessionTypeCheckingConsumer *checker= new SessionTypeCheckingConsumer();
        if (CI.hasASTContext()) {
          checker->Initialise(CI.getASTContext(), scribble_filename);
        }
        return checker;
      }

      bool ParseArgs(const CompilerInstance &CI,
                     const std::vector<std::string>& args) {
        if (args.size() == 0) {
          llvm::outs() << "Session Type Checker [sess-type-check] "
                       << "clang plugin\n";
          llvm::outs() << "Missing argument: "
                       << "Please specify Scribble file\n";
          return false;
        }

        if (args.size() == 1) {
          scribble_filename = std::string(args[0]);
          llvm::outs() << "Scribble: " << scribble_filename << "\n";
        }

        return true;
      }

    // end protected:

  }; // class SessionTypeCheckingAction

} // (null) namespace

static FrontendPluginRegistry::Add<SessionTypeCheckingAction>
X("sess-type-check", "Session type checker");
#ifdef __DEBUG__
        fprintf(stderr, "Normalise ST tree.\n");
#endif 

